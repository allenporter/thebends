//  Written by Allen Porter <allen@thebends.org>

#ifndef __RAMDISK_H__
#define __RAMDISK_H__

#include <map>
#include <string>
#include <fuse.h>

namespace ramdisk {

// TODO(allen): Support more than one ramdisk at a time
void NewRamdisk(struct fuse_operations* oper, size_t bytes);

}  // namespace ramdisk

#endif  // __RAMDISK_H__
