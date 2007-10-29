// ydb_op.h
// Author: Allen Porter <allen@thebends.org>
//
// Scheduled Db operations.
#ifndef _YDB_OP_H_
#define _YDB_OP_H_

#include <vector>
#include "ydb_base.h"

class Ydb_Map;

// Type of lock required by the operation
enum YLockType { READLOCK, WRITELOCK };

// Abstract db operation class.
class Ydb_Op {
 public:
  virtual ~Ydb_Op() { };

  // The type of lock required to execute this operation.
  virtual YLockType lock_type() const { return lock_type_; }

  // The key that the operation
  virtual const string& key() const { return key_; }

  // Executes the database operation.  The lock should be held prior to
  // calling this operation.
  virtual Ydb_RetCode Execute(Ydb_Map& ydb_map) = 0; 

  // Executes the inverse database operation, with the lock held.
  virtual void Rollback(Ydb_Map& ydb_map) = 0;

 protected:
  // Ydb_Op cannot be instantiated directly, see subclasses.
  Ydb_Op(YLockType lock_type, const string& key) :
    key_(key), lock_type_(lock_type) { }

  string key_;
  YLockType lock_type_;
};

typedef vector<Ydb_Op*> Ydb_Ops;

// Reads the current value of key into retval
class Ydb_ReadOp : public Ydb_Op {
 public:
  // caller is responsible for deleting retval
  Ydb_ReadOp(const string& key, string* retval);
  virtual ~Ydb_ReadOp() { };

  virtual Ydb_RetCode Execute(Ydb_Map& map);
  virtual void Rollback(Ydb_Map& map);

 private:
  string* retval_;
};

// Writes key/value pair
class Ydb_WriteOp : public Ydb_Op {
 public:
  Ydb_WriteOp(const string& key, const string& value);
  virtual ~Ydb_WriteOp() { };

  virtual Ydb_RetCode Execute(Ydb_Map& map);
  virtual void Rollback(Ydb_Map& map);

 private:
  string value_;       // value to write
  bool exists_;        // did the key exist before writing
  string orig_value_;  // original value
};

// Deletes a key from the database
class Ydb_DeleteOp : public Ydb_Op {
 public:
  Ydb_DeleteOp(const string& key);
  virtual ~Ydb_DeleteOp() { };

  virtual Ydb_RetCode Execute(Ydb_Map& map);
  virtual void Rollback(Ydb_Map& map);
 private:
  bool exists_;        // did the key exist before writing
  string orig_value_;  // original value
};

#endif  // _YDB_OP_H_
