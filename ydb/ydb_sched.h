// ydb_sched.h
// Author: Allen Porter <allen@thebends.org>
//
// Ydb Transaction scheduler.

#ifndef _YDB_SCHED_H_
#define _YDB_SCHED_H_

#include <vector>
#include <map>
#include "ydb_base.h"
#include "ydb_trans.h"

class YMutex;
class YCondVar;
class Ydb_DeadlockTree;
class Ydb_LockTable;
class Ydb_Map;
class Ydb_Op;

class Ydb_Scheduler {
 public:
  Ydb_Scheduler(Ydb_Map* map);
  ~Ydb_Scheduler();

  // Schedules the operation with the transaction.  The scheduler is
  // responsible for deallocating op.  The function will block until the
  // operation can be scheduled and the return value will be the return value
  // of the executed operation or DEADLOCK if the operation could not be
  // executed because it woudl cause a deadlock..
  Ydb_RetCode AddOp(Ydb_Trans* trans, Ydb_Op* op);

  // Creates a new Ydb_Trans obje
  Ydb_Trans* OpenTransaction();

  // Unlocks all the keys associated with the transaction and commits the
  // transaction.
  void Commit(Ydb_Trans* trans);

  // Reverts all operations previously performed by the transaction.
  void Rollback(Ydb_Trans* trans);

 private:

  Ydb_Map* map_;
  Ydb_LockTable* lock_table_;
  Ydb_DeadlockTree* deadlock_tree_;

  // mutex_ and condvar_ protect lock_table_ and tree_
  YMutex* mutex_;
  YCondVar* condvar_;
};

class Ydb_DeadlockTree {
 public:
  Ydb_DeadlockTree() { };
  ~Ydb_DeadlockTree() { };

  // Returns false if a deadlock occurs
  bool AddDependency(Ydb_Trans* new_trans, Ydb_Trans* existing);

  void Remove(Ydb_Trans* trans);

 private:
  typedef vector<Ydb_Trans*> TransList;
  typedef map<Ydb_Trans*, TransList*> DeadlockTable;

  bool IsBlockedBy(Ydb_Trans* new_trans, Ydb_Trans* existing);

  void RemoveFromList(TransList* list, Ydb_Trans* item);

  // maps a transaction to transactions that are blocking it
  DeadlockTable blocked_by_;

  // maps a transaction to transactions that are blocked by it
  DeadlockTable blocks_;
};


#endif  // _YDB_SCHED_H_
