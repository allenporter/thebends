// reader.h
// Author: Allen Porter <allen@thebends.org>
//
// TODO: test

#ifndef __YDB_READER_H__
#define __YDB_READER_H__

#include <string>

namespace ydb {

class Reader {
 public:
  virtual ~Reader() { }

  virtual bool ReadRecord(char* buf) = 0;

  virtual bool Eof() = 0;

  virtual bool Close() = 0;

 protected:
  // cannot be instantiated directly
  Reader() { }
};

Reader* OpenReader(const std::string& file, int record_size);

}  // namespace ydb

#endif  // __YDB_READER_H__
