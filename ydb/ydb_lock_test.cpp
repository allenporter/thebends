// ydb_trans_test.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Test basic functionality of the lock table.

#include <iostream>
#include <string>
#include "ydb_lock.h"
#include "ydb_trans.h"

using namespace std;

class Ydb_Lock_Test {
 public:
  static void test1();
  static void test2();
  static void test3();
};

// Object to let tests create fake transactions.
class TestTrans : public Ydb_Trans {
 public:
  TestTrans() : Ydb_Trans(NULL) { }
};

void Ydb_Lock_Test::test1() {
  // read and write locks for the same key by different transactions that
  // don't overloap.

  Ydb_LockTable table;
  Ydb_Trans* trans1 = new TestTrans();
  Ydb_Trans* trans2 = new TestTrans();

  assert(table.ReadLock(trans1, "key"));
  assert(table.Unlock(trans1, "key"));

  assert(table.ReadLock(trans2, "key"));
  assert(table.Unlock(trans2, "key"));

  assert(table.WriteLock(trans1, "key"));
  assert(table.Unlock(trans1, "key"));

  assert(table.WriteLock(trans2, "key"));
  assert(table.Unlock(trans2, "key"));

  assert(table.ReadLock(trans1, "key"));
  assert(table.Unlock(trans1, "key"));
}

void Ydb_Lock_Test::test2() {
  // test lock upgrading from read to write

  Ydb_LockTable table;
  Ydb_Trans* trans1 = new TestTrans();
  Ydb_Trans* trans2 = new TestTrans();

  // no locks are held
  assert(!table.Unlock(trans1, "key"));
  assert(table.GetLockHolders("key") == NULL);

  // upgrade read lock to a write lock
  assert(table.ReadLock(trans1, "key"));
  assert(table.WriteLock(trans1, "key"));
  assert(table.ReadLock(trans1, "key"));
  // trans1 holds a write lock
  assert(!table.ReadLock(trans2, "key"));
  const TransList* list1 = table.GetLockHolders("key");
  assert(list1->size() == 1);
  assert(list1->at(0) == trans1);

  assert(table.Unlock(trans1, "key"));
  // Unlock only needs to be called once per transaction
  assert(!table.Unlock(trans1, "key"));

  // test that all locks were removed
  assert(table.ReadLock(trans2, "key"));
  assert(table.Unlock(trans2, "key"));

  // unable to get a write lock because a read lock is held
  assert(table.ReadLock(trans1, "key"));
  assert(!table.WriteLock(trans2, "key"));
  const TransList* list2 = table.GetLockHolders("key");
  assert(list2->size() == 1);
  assert(list2->at(0) == trans1);
  assert(table.Unlock(trans1, "key"));

  // unable to upgrade because there are multiple readers
  assert(table.ReadLock(trans1, "key"));
  assert(table.ReadLock(trans2, "key"));
  assert(!table.WriteLock(trans2, "key"));
  const TransList* list3 = table.GetLockHolders("key");
  assert(list3->size() == 2);
  assert(table.Unlock(trans1, "key"));
  assert(table.Unlock(trans2, "key"));
}

void Ydb_Lock_Test::test3() {
  // test a shared read and exclusive write

  Ydb_LockTable table;
  Ydb_Trans* trans1 = new TestTrans();
  Ydb_Trans* trans2 = new TestTrans();

  // shared read
  assert(table.ReadLock(trans1, "key"));
  assert(table.ReadLock(trans2, "key"));
  assert(table.Unlock(trans1, "key"));
  assert(table.Unlock(trans2, "key"));

  assert(table.ReadLock(trans1, "key"));
  assert(!table.Unlock(trans2, "key"));
  assert(table.Unlock(trans1, "key"));

  // write is exclusive
  assert(table.WriteLock(trans1, "key"));
  assert(!table.ReadLock(trans2, "key"));
  assert(!table.WriteLock(trans2, "key"));
  assert(!table.Unlock(trans2, "key"));
  assert(table.Unlock(trans1, "key"));
}

int main(int argc, char* argv[]) {
  Ydb_Lock_Test test;
  test.test1();
  test.test2();
  test.test3();

  cout << "OK" << endl;
}
