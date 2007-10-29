#include <iostream>
#include "file.h"
#include "dir.h"

using namespace std;

namespace ramdisk {

File::File() : Node(S_IFREG | DEFAULT_PERMS), parent_(NULL), data_("") {
  stat_.st_nlink = 1;
}

File::File(Dir* parent, mode_t mode) : Node(S_IFREG | (PMASK & mode)),
                                       parent_(parent),
                                       data_("") {
  assert((mode & S_IFMT) == S_IFREG ||
         (mode & S_IFMT) == 0);
  stat_.st_nlink = 1;
}

File::~File() { }

size_t File::Read(char* buf, size_t size, off_t offset) {
  size_t len = data_.size();
  if (offset < len) {
    if (offset + size > len) {
      size = len - offset;
    }
    memcpy(buf, data_.data() + offset, size);
  } else {
    size = 0;
  }
  return size;
}

size_t File::Write(const char* buf, size_t size, off_t offset) {
  data_.replace(offset, size, buf, size);
  stat_.st_size = data_.size();
  parent_->UpdateDiskUsed();
  return size;
}

void File::Truncate(off_t offset) {
  data_.substr(0, offset);
  stat_.st_size = offset;
  parent_->UpdateDiskUsed();
}

}  // namespace ramdisk
