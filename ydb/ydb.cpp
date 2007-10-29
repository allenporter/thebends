#include <string>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>

#include "ydb.h"
#include "ydb_map.h"
#include "ydb_op.h"
#include "ydb_sched.h"
#include "ydb_trans.h"

const char* kDbFile = "ydb.db";
const char* kLogFile = "ydb.log";

Ydb::Ydb() : map_(NULL), db_dir_("") {
  // nothing to do
}

Ydb::~Ydb() {
  // TODO(allen) deallocate stuff, save database
  delete map_;
  delete sched_;
}

Ydb* Ydb::Open(const string& directory, bool init) {
  // TODO(aporter): Init
/*
  struct stat sb_dir;
  if (stat(directory.c_str(), &sb_dir) < 0) {
    perror("Ydb::open()");
    return NULL;
  }
  if ((sb_dir.st_mode & S_IFDIR) != S_IFDIR) {
    fprintf(stderr, "Ydb::open(): Not a directory '%s'\n", directory.c_str());
    return NULL;
  }
*/
  string dbfile = directory;
  dbfile.append("/");
  dbfile.append(kDbFile);

  string logfile = directory;
  logfile.append("/");
  logfile.append(kLogFile);

  // read database
  Ydb_Map *map = Ydb_Map::Open(dbfile);
  if (map == NULL) {
    fprintf(stderr, "Ydb::Open(): Error while opening db '%s'\n",
            dbfile.c_str());
    return NULL;
  }

  Ydb *db = new Ydb;
  db->map_ = map;
  db->db_dir_ = directory;
  db->sched_ = new Ydb_Scheduler(db->map_);
  return db;
}

Ydb_Trans* Ydb::OpenTransaction() {
  return sched_->OpenTransaction();
}

Ydb_RetCode Ydb::Get(const string& key, string* value) {
  Ydb_Trans* trans = OpenTransaction();
  Ydb_RetCode ret = Get(key, value, trans);
  trans->Commit();
  return ret;
}

Ydb_RetCode Ydb::Get(const string& key, string* value, Ydb_Trans* trans) {
  return sched_->AddOp(trans, new Ydb_ReadOp(key, value));
}

Ydb_RetCode Ydb::Put(const string& key, const string& value) {
  Ydb_Trans* trans = OpenTransaction();
  Ydb_RetCode ret = Put(key, value, trans);
  trans->Commit();
  return ret;
}

Ydb_RetCode Ydb::Put(const string& key, const string& value, Ydb_Trans* trans) {
  return sched_->AddOp(trans, new Ydb_WriteOp(key, value));
}

Ydb_RetCode Ydb::Del(const string& key) {
  Ydb_Trans* trans = OpenTransaction();
  Ydb_RetCode ret = Del(key, trans);
  trans->Commit();
  return ret;
}

Ydb_RetCode Ydb::Del(const string& key, Ydb_Trans* trans) {
  return sched_->AddOp(trans, new Ydb_DeleteOp(key));
}
