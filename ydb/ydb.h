// ydb.h
// Author: Allen Porter <allen@thebends.org>
//
// Y Database - A simple key/value pair database.
//
// TODO(allen) Describe:

#ifndef _YDB_H_
#define _YDB_H_

#include "ydb_base.h"
#include "ydb_map.h"
#include "ydb_sched.h"
#include "ydb_trans.h"

// Ydb interface
class Ydb {
 public:
  // Opens the database instance located in the specified directory.  If init
  // is true, will create the database instance if it does not already exist.
  static Ydb* Open(const string& directory, bool init);

  // Creates a new transaction.  The caller is responsible for deallocation of
  // the returned transaction.
  Ydb_Trans* OpenTransaction();

  // For the specified key, will return a value if it exists.  If there is no
  // no value for the key, the return result will be false.  If the key does
  // exist, it will be copied into value and the return result is true.
  Ydb_RetCode Get(const string& key, string* value);
  Ydb_RetCode Get(const string& key, string* value, Ydb_Trans *trans);

  // Updates the key with the specified value.  Returns false if the update
  // coudl not be processed.
  Ydb_RetCode Put(const string& key, const string& value);
  Ydb_RetCode Put(const string& key, const string& value, Ydb_Trans *trans);

  // Removes the specified key and returns true if it was removed successfully.
  // False is returned if the key does not already exist.
  Ydb_RetCode Del(const string& key);
  Ydb_RetCode Del(const string& key, Ydb_Trans* trans);

 private:
  // Ydb cannot be instantiated directly. See open() static method.
  Ydb();
  ~Ydb();

  Ydb_Map* map_;         // key value pairs
  string db_dir_;        // db instance directory
  Ydb_Scheduler* sched_;
};

#endif // _YDB_H_
