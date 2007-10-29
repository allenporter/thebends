// ydb_map.cpp
// Author: Allen Porter <allen@thebends.org>

#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include "ydb_map.h"

Ydb_Map::Ydb_Map() : db_file_("") {
}

Ydb_Map::~Ydb_Map() {
}

Ydb_Map* Ydb_Map::Open(const string& filename) {
  Ydb_Map *ydb_map = new Ydb_Map;
  ydb_map->db_file_ = filename;
  return ydb_map;
}

bool Ydb_Map::Get(const string& key, string* value) {
  ythread::MutexLock l(&mutex_);
  return db_.Get(key, value);
/*
  ymap::iterator iter = db_.find(key);  
  if (iter == db_.end()) {
    return false;
  }
  *value = iter->second;
  return true;
*/
}

void Ydb_Map::Put(const string& key, const string& value) {
  ythread::MutexLock l(&mutex_);
  db_.Put(key, value);
//  db_[key] = value;
}

bool Ydb_Map::Del(const string& key) {
  ythread::MutexLock l(&mutex_);
  return db_.Del(key);
/*
  ymap::iterator iter = db_.find(key);  
  if (iter == db_.end()) {
    return false;
  }
  db_.erase(iter);
  return true;
*/
}

void Ydb_Map::Dump() {
/*
  YDEBUG("===START DUMP================================================");
  for (ymap::iterator iter = db_.begin(); iter != db_.end(); ++iter) {
    const string& key = iter->first;
    const string& value = iter->second;
    YDEBUG("DUMP: [" << key << "] = '" << value << "'");
  }
  YDEBUG("===END DUMP==================================================");
*/
}
