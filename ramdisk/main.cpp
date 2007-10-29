/*
 * Ramdisk for FUSE: Filesystem in Userspace
 * Allen Porter <allen@thebends.org>
 */

#include "ramdisk.h"

static int kDiskSize = 100 * 1024 * 1024;

static struct fuse_operations ramdiskfs_oper;

int main(int argc, char *argv[]) {
  ramdisk::NewRamdisk(&ramdiskfs_oper, kDiskSize);
  return fuse_main(argc, argv, &ramdiskfs_oper, NULL);
}
