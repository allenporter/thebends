#include "assert.h"
#include "ydb.h"

#define TESTDIR "/tmp/ydb";

#define assertOk(x) assert(YDB_OK == (x))
#define assertNotFound(x) assert(YDB_NOT_FOUND == (x))
#define assertDeadlock(x) assert(YDB_DEADLOCK == (x))

void test_simple(Ydb& db) {
  string key = "test_key";
  string value = "test_value";

  string v;
  assertNotFound(db.Del(key));
  assertNotFound(db.Get(key, &v));

  assertOk(db.Put(key, value));
  assertOk(db.Get(key, &v));
  assert(value == v);

  assertOk(db.Del(key));

  assertNotFound(db.Get(key, &v));
}

void test_trans1(Ydb& db) {
  string key = "trans1";
  string value;

  assertOk(db.Put(key, "a"));
  assertOk(db.Get(key, &value));
  assert(value == "a");

  // committed transaction
  Ydb_Trans* trans = db.OpenTransaction();
  assert(trans != NULL);
  string value2;
  assertOk(db.Get(key, &value2, trans));
  assert(value2 == "a");
  assertOk(db.Put(key, "b", trans));
  assertOk(db.Get(key, &value2, trans));
  assert(value2 == "b");
  trans->Commit();

  string value3;
  assertOk(db.Get(key, &value3));
  assert(value3 == "b");

  // aborted transaction  
  trans = db.OpenTransaction();
  assert(trans != NULL);
  string value4;
  assertOk(db.Get(key, &value4, trans));
  assert(value4 == "b");
  assertOk(db.Put(key, "c", trans));
  assertOk(db.Get(key, &value4, trans));
  assert(value4 == "c");
  trans->Abort();

  string value5;
  assertOk(db.Get(key, &value5));
  assert(value5 == "b");
}

void test_trans2(Ydb& db) {
  Ydb_Trans* trans1 = db.OpenTransaction();
  Ydb_Trans* trans2 = db.OpenTransaction();

  assertOk(db.Put("key-a", "a", trans1));
  trans1->Commit();

  assertOk(db.Put("key-a", "b", trans2));
  trans2->Commit();

  string val;
  assertOk(db.Get("key-a", &val));
  assert(val == "b");
}

int main(int argc, char *argv[]) {
  const string& dir = TESTDIR;

  Ydb *db = Ydb::Open(dir, true);
  assert(db != NULL);
  test_simple(*db);
  test_trans1(*db);
  test_trans2(*db);

  cout << "OK\n";
}
