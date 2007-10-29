// ydb_trans.cpp
// Author: Allen Porter <allen@thebends.org>

#include "ydb_op.h"
#include "ydb_sched.h"
#include "ydb_trans.h"

Ydb_Trans::~Ydb_Trans() {
  // Transaction must be committed or aborted before deleting
  assert(state_ == COMMITTED || state_ == ABORTED);

  for (Ydb_Ops::iterator iter = ops_.begin();
       iter != ops_.end();
       ++iter) {
    delete *iter;
  }
  ops_.clear();
}

void Ydb_Trans::Commit() {
  // Ask the scheduler to commit all the operations in the transaction
  sched_->Commit(this);
}

void Ydb_Trans::Abort() {
  // Ask the scheduler to remove all the operations in the transaction
  sched_->Rollback(this);
}

void Ydb_Trans::AddOp(Ydb_Op* op) {
  ops_.push_back(op);
}
