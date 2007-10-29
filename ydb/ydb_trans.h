// ydb_trans.h
// Author: Allen Porter <allen@thebends.org>
//
// Ydb_Trans holds state for an open transaction.  Transactions are created by
// Ydb and ,
// modified, and deleted by the Ydb_Scheduler.
//
// Ydb_Trans* trans = db->OpenTransaction();
// db->Put("A", "1", trans);
// db->Put("B", "2", trans);
// db->Del("C", trans);
// trans->Commit();    // deallocates the object
//

#ifndef _YDB_TRANS_H_
#define _YDB_TRANS_H_

#include <vector>
#include "ydb_op.h"

class Ydb_Trans;
typedef vector<Ydb_Trans*> TransList;

class Ydb_Trans {
 public:
  ~Ydb_Trans();

  // Commits all operations associated with this transaction and deletes the
  // transaction.  The transaction object should not be used after calling
  // Commit.
  void Commit();

  // Aborts all operations associated with this transaction and deletes the
  // transaction.  The transaction object should not be used after calling
  // Abort.
  void Abort();

 protected:
  friend class Ydb_Scheduler;  // for AddOp and GetOps

  enum TransactionState { OPEN = 0, ABORTED, COMMITTED };

  // Ydb_Trans cannot be instantiated directly.
  Ydb_Trans(Ydb_Scheduler* sched) : sched_(sched), state_(OPEN) { };

  // Adds the operation to the transaction.  The transaction takes ownership
  // of the operation and will take care of deallocation.
  void AddOp(Ydb_Op* op);

  // Returns the list of operations associated with this transaction.
  Ydb_Ops& GetOps() { return ops_; }

  Ydb_Scheduler* sched_;
  TransactionState state_;
  Ydb_Ops ops_;
};

#endif  // _YDB_TRANS_H_
