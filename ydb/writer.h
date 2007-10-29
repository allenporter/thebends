// writer.h
// Author: Allen Porter <allen@thebends.org>
//
// TODO: test

#ifndef __YDB_WRITER_H__
#define __YDB_WRITER_H__

#include <string>

namespace ydb {

class Writer {
 public:
  virtual ~Writer() { }

  virtual bool WriteRecord(char* buf) = 0;

  virtual bool Close() = 0;

 protected:
  // cannot be instantiated directly
  Writer() { }
};

Writer* OpenWriter(const std::string& file, int record_size);

}  // namespace ydb

#endif  // __YDB_WRITER_H__
