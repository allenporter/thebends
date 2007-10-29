#undef NDEBUG

#include "ramdisk.h"
#include <vector>
#include <string>
#include <fuse.h>
#include <stdio.h>
#include <sys/stat.h>

static std::vector<std::string> files;

static int filler(void* buf, const char* path, const struct stat* stat,
                  off_t off) {
  files.push_back(path);
  return 0;
}

void test_readdir() {
  struct fuse_operations ramdiskfs_oper;
  ramdisk::Ramdisk::NewRamdisk(&ramdiskfs_oper);
  int ret;

  ret = ramdiskfs_oper.readdir("/", NULL, filler, 0, NULL);
  assert(ret == 0);
  assert(files.size() == 2);
  assert(files[0] == ".");
  assert(files[1] == "..");

  struct fuse_file_info fi_foo;
  ret = ramdiskfs_oper.create("/foo", 0777, &fi_foo);
  assert(ret == 0);
  struct fuse_file_info fi_bar;
  ret = ramdiskfs_oper.create("/bar", 0777, &fi_bar);
  assert(ret == 0);
  files.clear();
  ret = ramdiskfs_oper.readdir("/", NULL, filler, 0, NULL);
  assert(ret == 0);
  assert(files.size() == 4);
  assert(files[0] == ".");
  assert(files[1] == "..");
  assert(files[2] == "bar");
  assert(files[3] == "foo");

  ret = ramdiskfs_oper.unlink("/foo");
  assert(ret == 0);
  files.clear();
  ret = ramdiskfs_oper.readdir("/", NULL, filler, 0, NULL);
  assert(ret == 0);
  assert(files.size() == 3);
  assert(files[0] == ".");
  assert(files[1] == "..");
  assert(files[2] == "bar");

  ret = ramdiskfs_oper.unlink("/bar");
  assert(ret == 0);
  files.clear();
  ret = ramdiskfs_oper.readdir("/", NULL, filler, 0, NULL);
  assert(ret == 0);
  assert(files.size() == 2);
  assert(files[0] == ".");
  assert(files[1] == "..");
}

void test_getattr() {
  struct fuse_operations ramdiskfs_oper;
  ramdisk::Ramdisk::NewRamdisk(&ramdiskfs_oper);

  int ret;
  struct stat stbuf;
  memset(&stbuf, 0, sizeof(struct stat));

  ret = ramdiskfs_oper.getattr("/", &stbuf);
  assert(ret == 0);
  assert(stbuf.st_mode | S_IFDIR != 0);
  assert(stbuf.st_nlink == 2);
}

void test_readwrite() {
  struct fuse_operations ramdiskfs_oper;
  ramdisk::Ramdisk::NewRamdisk(&ramdiskfs_oper);
  int ret;

  struct fuse_file_info fi_abc;
  ret = ramdiskfs_oper.create("/abc", 0777, &fi_abc);
  assert(ret == 0);
  ret = ramdiskfs_oper.write("/abc", "123456\n", 7, 0, &fi_abc);
  assert(ret == 7);
  ret = ramdiskfs_oper.release("/abc",  &fi_abc);
  assert(ret == 0);

  ret = ramdiskfs_oper.open("/abc", &fi_abc);
  assert(ret == 0);
  char buf[16];
  ret = ramdiskfs_oper.read("/abc", buf, 7, 0, &fi_abc);
  assert(ret == 7);
  buf[7] = 0;
  assert(strcmp("123456\n", buf) == 0);

  ret = ramdiskfs_oper.release("/abc", &fi_abc);
  assert(ret == 0);
}

void test_mkdir() {
  struct fuse_operations ramdiskfs_oper;
  ramdisk::Ramdisk::NewRamdisk(&ramdiskfs_oper);
  int ret;

  ret = ramdiskfs_oper.mkdir("/untitled folder", 040755);

  files.clear();
  ret = ramdiskfs_oper.readdir("/", NULL, filler, 0, NULL);
  assert(ret == 0);
  assert(files.size() == 3);
  assert(files[0] == ".");
  assert(files[1] == "..");
  assert(files[2] == "untitled folder");

  files.clear();
  ret = ramdiskfs_oper.readdir("/untitled folder", NULL, filler, 0, NULL);
  assert(ret == 0);
  assert(files.size() == 2);
  assert(files[0] == ".");
  assert(files[1] == "..");
}

int main(int argc, char* argv[]) {
  test_readdir();
  test_getattr();
  test_readwrite();
  test_mkdir();
  printf("PASS\n");
  return 0;
}
