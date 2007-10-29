// ydb_op.cpp
// Author: Allen Porter <allen@thebends.org>

#include <map>
#include "ydb_lock.h"
#include "ydb_map.h"
#include "ydb_op.h"

Ydb_ReadOp::Ydb_ReadOp(const string& key, string* retval)
  : Ydb_Op(READLOCK, key), retval_(retval)
{ }

Ydb_RetCode Ydb_ReadOp::Execute(Ydb_Map& map) {
  if (map.Get(key_, retval_)) {
    return YDB_OK;
  } else {
    return YDB_NOT_FOUND;
  }
}

void Ydb_ReadOp::Rollback(Ydb_Map& map) {
  // nothing to do
}

Ydb_WriteOp::Ydb_WriteOp(const string& key, const string& value)
  : Ydb_Op(WRITELOCK, key), value_(value)
{ }

Ydb_RetCode Ydb_WriteOp::Execute(Ydb_Map& map) {
  exists_ = map.Get(key_, &orig_value_);
  map.Put(key_, value_);
  return YDB_OK;
}

void Ydb_WriteOp::Rollback(Ydb_Map& map) {
  if (exists_) {
    map.Put(key_, orig_value_);
  }
}

Ydb_DeleteOp::Ydb_DeleteOp(const string& key)
  : Ydb_Op(WRITELOCK, key)
{ }

Ydb_RetCode Ydb_DeleteOp::Execute(Ydb_Map& map) {
  exists_ = map.Get(key_, &orig_value_);
  if (exists_) {
    map.Del(key_);
    return YDB_OK;
  } else {
    return YDB_NOT_FOUND;
  }
}

void Ydb_DeleteOp::Rollback(Ydb_Map& map) {
  if (exists_) {
    map.Put(key_, orig_value_);
  }
}
