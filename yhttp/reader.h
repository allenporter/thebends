// reader.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __YHTTPSERVER_READER_H__
#define __YHTTPSERVER_READER_H__

#include <string>

namespace yhttpserver {

class Reader {
 public:
  Reader(int fd);
  ~Reader();

  bool Read();

  const std::string& Buffer() { return buf_; }

 protected:
  int sock_;
  std::string buf_;
};

}  // namespace yhttpserver

#endif  // __YHTTPSERVER_READER_H__
