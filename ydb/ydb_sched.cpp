// ydb_sched.cpp
// Author: Allen Porter <allen@thebends.org>

#include <iostream>
#include <unistd.h>
#include <ythread/mutex.h>
#include <ythread/condvar.h>
#include "ydb_lock.h"
#include "ydb_trans.h"
#include "ydb_op.h"
#include "ydb_sched.h"

Ydb_Scheduler::Ydb_Scheduler(Ydb_Map* map)
  : map_(map), lock_table_(new Ydb_LockTable()),
    deadlock_tree_(new Ydb_DeadlockTree()),
    mutex_(new ythread::Mutex()),
    condvar_(new ythread::CondVar(mutex_)) { }

Ydb_Scheduler::~Ydb_Scheduler() {
  delete lock_table_;
  delete deadlock_tree_;
  delete condvar_;
  delete mutex_;
}

Ydb_RetCode Ydb_Scheduler::AddOp(Ydb_Trans* trans, Ydb_Op* op) {
  // TODO(aporter): Upgrading a lock cause it to be unlocked only once?
  bool ret = false;

  mutex_->Lock();
  do {
    // TODO(aporter): get a result code indicating that the lock is held or
    // that a deadlock would occur
    ret = lock_table_->Lock(trans, op);
    if (!ret) {
      // Another transaction already holds a lock on the key.  To prevent
      // deadlocks, we check for a cycle in a dependancy graph.
      const TransList* trans_list = lock_table_->GetLockHolders(op->key());
      assert(trans_list->size() != 0);

      // For each transaction
      for (TransList::const_iterator iter = trans_list->begin();
           iter != trans_list->end(); ++iter) {
        if (!deadlock_tree_->AddDependency(*iter, trans)) {
	  // Blocking this transaction will cause a deadlock, so it must abort
          mutex_->Unlock();
          Rollback(trans);
	  return YDB_DEADLOCK;
        }
      }
      condvar_->Wait();  // atomicly releses and re-grabs mutex_
    }
  } while (!ret);
  mutex_->Unlock();

  trans->AddOp(op);
  return op->Execute(*map_);
}

Ydb_Trans* Ydb_Scheduler::OpenTransaction() {
  return new Ydb_Trans(this);
}

// TODO(aporter): Unlock in the reverse locking order?
void Ydb_Scheduler::Commit(Ydb_Trans* trans) {
  Ydb_Ops& ops = trans->GetOps();
  mutex_->Lock();
  for (Ydb_Ops::iterator iter = ops.begin();
       iter != ops.end();
       ++iter) {
    Ydb_Op* op = *iter;
    lock_table_->Unlock(trans, op->key());
  }
  deadlock_tree_->Remove(trans);
  condvar_->SignalAll();
  mutex_->Unlock();
}

void Ydb_Scheduler::Rollback(Ydb_Trans* trans) {
  Ydb_Ops& ops = trans->GetOps();
  mutex_->Lock();
  for (Ydb_Ops::iterator iter = ops.begin();
       iter != ops.end();
       ++iter) {
    Ydb_Op* op = *iter;
    op->Rollback(*map_);
    lock_table_->Unlock(trans, op->key());
  }
  deadlock_tree_->Remove(trans);
  condvar_->SignalAll();
  mutex_->Unlock();
}

// Is existing blocked by new_trans?
bool Ydb_DeadlockTree::IsBlockedBy(Ydb_Trans* existing, Ydb_Trans* new_trans) {
  DeadlockTable::iterator iter = blocked_by_.find(existing);
  if (iter == blocked_by_.end()) {
    return false;
  }

  TransList* list = iter->second;
  for (TransList::iterator t = list->begin(); t != list->end(); ++t) {
    if (*t == new_trans) {
      return true;
    } else {
      if (IsBlockedBy(new_trans, *t)) {
        return true;
      }
    }
  }
  return false;
}

bool Ydb_DeadlockTree::AddDependency(Ydb_Trans* existing,
                                     Ydb_Trans* new_trans) {
  if (IsBlockedBy(existing, new_trans)) {
    // Adding this dependency would cause a deadlock
    return false;
  }

  // Mark that new_trans is blocked by existing
  DeadlockTable::iterator iter = blocked_by_.find(new_trans);
  TransList* list;
  if (iter == blocked_by_.end()) {
    list = new TransList();
    blocked_by_[new_trans] = list;
  } else {
    list = iter->second;
  }
  list->push_back(existing);

  // Mark that existing is blocking new_trans
  DeadlockTable::iterator blocks_iter = blocks_.find(existing);
  TransList* blocks_list;
  if (blocks_iter == blocks_.end()) {
    blocks_list = new TransList();
    blocks_[existing] = blocks_list;
  } else {
    blocks_list = blocks_iter->second;
  }
  blocks_list->push_back(new_trans);

  return true;
}

void Ydb_DeadlockTree::Remove(Ydb_Trans* trans) {
  DeadlockTable::iterator iter = blocks_.find(trans);
  if (iter == blocks_.end()) {
    return;
  }

  // Remove this transaction from the list of blocked by transactions for each
  // transaction waiting on it.
  TransList* translist = iter->second;
  for (TransList::iterator trans_iter = translist->begin();
       trans_iter != translist->end(); ++trans_iter) {
    DeadlockTable::iterator blocked_iter = blocked_by_.find(*trans_iter);
    assert(blocked_iter != blocked_by_.end());
    TransList* list = blocked_iter->second;
    RemoveFromList(list, trans);
  }
  blocks_.erase(iter);
  delete translist;
}

void Ydb_DeadlockTree::RemoveFromList(TransList* list, Ydb_Trans* item) {
  for (TransList::iterator iter = list->begin();
       iter != list->end(); ++iter) {
    if (*iter == item) {
      list->erase(iter);
      return;
    }
  }
}
