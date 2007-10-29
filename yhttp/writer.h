// writer.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __YHTTPSERVER_WRITER_H__
#define __YHTTPSERVER_WRITER_H__

#include <string>

namespace yhttpserver {

class Writer {
 public:
  Writer(int fd);
  ~Writer();

  void WriteString(const std::string& message);
  void WriteString(const char* message, size_t size);

  bool Flush();

 protected:
  int sock_;
  std::string buf_;
};

}  // namespace yhttpserver

#endif  // __YHTTPSERVER_WRITER_H__
