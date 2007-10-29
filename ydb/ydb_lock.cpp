// ydb_lock.cpp
// Author: Allen Porter <allen@thebends.org>

#include "ydb_lock.h"
#include <map>
#include <vector>

Ydb_LockTable::Ydb_LockTable() { } 

Ydb_LockTable::~Ydb_LockTable() {
  LockMap::iterator iter;
  for (iter = lock_map_.begin(); iter != lock_map_.end(); ++iter) {
    delete iter->second;
  }
  lock_map_.clear();
}

bool Ydb_LockTable::Lock(Ydb_Trans* trans, Ydb_Op* op) {
  if (op->lock_type() == READLOCK) {
    return ReadLock(trans, op->key());
  } else {  // op->lock_type() == WRITELOCK
    return WriteLock(trans, op->key());
  }
}

bool Ydb_LockTable::ReadLock(Ydb_Trans* trans, string item) {
  ythread::MutexLock l(&mutex_);
  LockMap::iterator iter = lock_map_.find(item);
  if (iter == lock_map_.end()) {
    // lockinfo is deleted when the lock is released
    Ydb_LockInfo* info = new Ydb_LockInfo(READLOCK);
    info->add_transaction(trans);
    lock_map_[item] = info;
    return true;
  }

  // another transaction holds a lock on this item.  if it is a read lock,
  // then we'll add this transaction to the list of readers holding a lock.  if
  // the existing lock is a write lock, then this transaction will be denied
  // access to hold a read lock.  alternatively, we can allow this lock if the
  // this same transaction holds a write lock.
  Ydb_LockInfo* info = iter->second;
  if (info->type() == READLOCK) {
    info->add_transaction(trans); 
    return true;
  }

  assert(info->type() == WRITELOCK);
  if (info->transaction_count() == 1 && info->has_transaction(trans)) {
    // this transaction already has a write lock, so no read lock needed.
    return true;
  }
  return false;
}

// A write lock is exclusive and can be obtained if no other locks are head on
// the item or if the transaction that holds a read lock is requesting an
// upgrade to a write lock.
bool Ydb_LockTable::WriteLock(Ydb_Trans* trans, string item) {
  ythread::MutexLock l(&mutex_);
  LockMap::iterator iter = lock_map_.find(item);
  if (iter == lock_map_.end()) {
    // lockinfo is deleted when the lock is released
    Ydb_LockInfo* info = new Ydb_LockInfo(WRITELOCK);
    info->add_transaction(trans);
    lock_map_[item] = info;
    return true;
  }

  Ydb_LockInfo* info = iter->second;
  if (info->type() == READLOCK && info->transaction_count() == 1 &&
      info->remove_transaction(trans)) {
    // upgrade to a write lock
    info->set_type(WRITELOCK);
    info->add_transaction(trans);
    return true;
  }
  return false;
}

const TransList* Ydb_LockTable::GetLockHolders(string item) {
  ythread::MutexLock l(&mutex_);
  LockMap::iterator iter = lock_map_.find(item);
  if (iter == lock_map_.end()) {
    return NULL;
  }
  Ydb_LockInfo* info = iter->second;
  return &info->transactions();
}

bool Ydb_LockTable::Unlock(Ydb_Trans* trans, string item) {
  ythread::MutexLock l(&mutex_);
  LockMap::iterator iter = lock_map_.find(item);
  if (iter == lock_map_.end()) {
    return false;
  }

  Ydb_LockInfo* info = iter->second;
  if (!info->remove_transaction(trans)) {
    return false;
  }

  if (info->transaction_count() == 0) {
    lock_map_.erase(item);
    delete info;
  }

  return true;
}

Ydb_LockInfo::Ydb_LockInfo(YLockType type) : type_(type) { }
Ydb_LockInfo::~Ydb_LockInfo() { }

YLockType Ydb_LockInfo::type() {
  return type_;
}

void Ydb_LockInfo::set_type(YLockType type) {
  assert(type_ == READLOCK && type == WRITELOCK);
  type_ = type;
}

void Ydb_LockInfo::add_transaction(Ydb_Trans* trans) {
  transactions_.push_back(trans); 
}

bool Ydb_LockInfo::has_transaction(Ydb_Trans* trans) {
  TransList::iterator iter;
  bool found = false;
  for (iter = transactions_.begin(); iter != transactions_.end(); ++iter) {
    if (*iter == trans) {
      found = true;
      break;
    }
  }
  return found;
}

bool Ydb_LockInfo::remove_transaction(Ydb_Trans* trans) {
  TransList::iterator iter;
  bool found = false;
  for (iter = transactions_.begin(); iter != transactions_.end(); ++iter) {
    if (*iter == trans) {
      found = true;
      break;
    }
  }
  if (found) {
    transactions_.erase(iter, iter+1);
    return true;
  }
  return false;
}
