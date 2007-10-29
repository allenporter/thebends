// Written by Allen Porter <allen@thebends.org>
// TODO: Notify parents of file size changes

#ifndef __RAMDISK_NODE_H__
#define __RAMDISK_NODE_H__

#include <fuse.h>
#include <sys/stat.h>

namespace ramdisk {

#define UMASK (S_IWGRP | S_IWOTH)

#define PMASK (S_IRWXU | S_IRWXG | S_IRWXO)
#define DEFAULT_PERMS (PMASK & ~UMASK)

class Node {
 public:
  Node(mode_t mode);
  virtual ~Node();

  virtual void Chmod(mode_t mode);
  virtual void Chown(uid_t uid, gid_t gid);
  virtual void GetAttr(struct stat* stbuf);
  virtual size_t Size() const;

 protected:
  struct stat stat_;
};

}  // namespace ramdisk

#endif  // __RAMDISK_NODE_H__
