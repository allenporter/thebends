// Written by Allen Porter <allen@thebends.org>

#ifndef __RAMDISK_FILE_H__
#define __RAMDISK_FILE_H__

#include <string>
#include "node.h"

namespace ramdisk {

class Dir;

class File : public Node {
 public:
  File();
  File(Dir* parent, mode_t mode);
  virtual ~File();

  virtual size_t Read(char* buf, size_t size, off_t offset);
  virtual size_t Write(const char* buf, size_t size, off_t offset);
  virtual void Truncate(off_t offset);

 private:
  Dir* parent_;
  std::string data_;
};

}  // namespace ramdisk

#endif  // __RAMDISK_FILE_H__
