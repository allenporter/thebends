// ydb_map.h
// Author: Allen Porter <allen@thebends.org>
//
// A mapping of string key/value pairs, the underlying data storage for ydb.  A
// Ydb_Map can be accessed concurrently by multiple threads after the
// initialization, but relies on Ydb_LockTable to protect concurrency of the
// underlying data.
//
// TODO(aporter): Checkpoint command?

#ifndef _YDB_MAP_H_
#define _YDB_MAP_H_

//#include <map>
#include "ymutex.h"
#include "ydb_base.h"
#include "ydb_tree.h"

class Ydb_Map {
 public:
  ~Ydb_Map();

  static Ydb_Map* Open(const string& directory);

  // Returns the value for the key.  Returns false is the key does not exist.
  bool Get(const string& key, string* value);

  // Updates the key with the value.
  void Put(const string& key, const string& value);

  // Deletes the key.  Returns false if the key did not already exist.
  bool Del(const string& key);

  void Dump();

 private:
//  typedef map<string, string> ymap;

  // Ydb_Map cannot be instantiated directly. See open() static method.
  Ydb_Map();

  YMutex mutex_;
  Ydb_Tree db_;
//  ymap db_;
  string db_file_;
};

#endif  // _YDB_MAP_H_
