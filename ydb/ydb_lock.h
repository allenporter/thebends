// ydb_lock.h
// Author: Allen Porter <allen@thebends.org>

#ifndef _YDB_LOCK_H_
#define _YDB_LOCK_H_

#include <map>
#include "ymutex.h"
#include "ydb_base.h"
#include "ydb_op.h"
#include "ydb_trans.h"

class Ydb_LockInfo;

class Ydb_LockTable {
 public:
  Ydb_LockTable();
  ~Ydb_LockTable();

  // TODO(allen): Document these functions
  bool Lock(Ydb_Trans* transaction, Ydb_Op* op);
  bool ReadLock(Ydb_Trans* transaction, string item);
  bool WriteLock(Ydb_Trans* transaction, string item);
  bool Unlock(Ydb_Trans* transaction, string item);

  // GetLockHolders
  //   Returns a list of transactions that currently hold a lock for the
  //   specified item.  The list is only valid if no other calls to ReadLock,
  //   WriteLock, or Unlcok are made.  NULL is returned if there are no lock
  //   holders for the item.
  const TransList* GetLockHolders(string item);

 private:
  typedef map<string, Ydb_LockInfo*> LockMap;

  YMutex mutex_;      // protects lock_map_
  LockMap lock_map_;
};

// Ydb_LockInfo holds info such as what type of lock is held on a key and which
// transactions are involved in the lock.  Ydb_LockInfo does not enforce
// constraints such as the number of transactions for a particular lock type.
class Ydb_LockInfo {
 public:
  Ydb_LockInfo(YLockType type);
  ~Ydb_LockInfo();

  YLockType type();
  void set_type(YLockType type);

  // TODO(aporter): fix in class def
  void add_transaction(Ydb_Trans* transaction);
  bool has_transaction(Ydb_Trans* transaction);
  bool remove_transaction(Ydb_Trans* transaction);
  int transaction_count() { return transactions_.size(); };
  const TransList& transactions() { return transactions_; };

 private:
  YLockType type_;          // type of lock
  TransList transactions_;  // all transactions holding this lock
};

#endif // _YDB_LOCK_H_
