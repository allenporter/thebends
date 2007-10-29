#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "node.h"

using namespace std;

namespace ramdisk {

Node::Node(mode_t mode) {
  memset(&stat_, 0, sizeof(struct stat));
  stat_.st_mode = mode;
  stat_.st_uid = getuid();
  stat_.st_gid = getgid();
}

Node::~Node() { }

void Node::Chmod(mode_t mode) {
  // Can't set file type; only file mode
  assert((mode & S_IFMT) == 0);
  stat_.st_mode = (stat_.st_mode & S_IFMT) | mode;
}

void Node::Chown(uid_t uid, gid_t gid) {
  stat_.st_uid = uid;
  stat_.st_gid = gid;
}

void Node::GetAttr(struct stat* stbuf) {
  *stbuf = stat_;
}

size_t Node::Size() const {
  return stat_.st_size;
}

}  // namespace ramdisk
