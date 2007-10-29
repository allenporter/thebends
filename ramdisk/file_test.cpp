#include <stdlib.h>
#include <iostream>
#include "file.h"

using namespace std;

void test_readwrite() {
  static const size_t kLen = 10;
  char buf[kLen];
  ramdisk::File file;
  for (size_t i = 0; i < kLen; ++i) {
    buf[i] = (char)random();
  }
  file.Write(buf, kLen, 0);
  for (size_t size = 1; size < kLen; ++size) {
    char actual_buf[kLen];
    off_t offset;
    for (offset = 0; offset < kLen; offset += size) {
      size_t read_size = size;
      if (size + offset > kLen) {
        read_size = kLen - offset;
      }
      assert(file.Read(&actual_buf[offset], read_size, offset) == read_size);
    }
    assert(memcmp(&actual_buf, &buf, kLen) == 0);
  }
}

void test_size() {
  ramdisk::File file;
  assert(file.Size() == 0);
  file.Write("abc", 3, 0); 
  assert(file.Size() == 3);
  file.Write("d", 1, 3);
  assert(file.Size() == 4);
  file.Truncate(3);
  assert(file.Size() == 3);
  file.Write("zzzz", 4, 1); 
  assert(file.Size() == 5);
  file.Truncate(0);
  assert(file.Size() == 0);
}

void test_getattr() {
  struct stat st;
  ramdisk::File file;
  file.GetAttr(&st);
  assert((st.st_mode & S_IFMT) == S_IFREG);

  file.Chmod(0765);;
  file.GetAttr(&st);
  assert((st.st_mode & S_IFMT) == S_IFREG);
  assert((st.st_mode & 0777) == 0765);

  file.Chmod(0111);;
  file.GetAttr(&st);
  assert((st.st_mode & S_IFMT) == S_IFREG);
  assert((st.st_mode & 0777) == 0111);
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  test_readwrite();
  test_size();
  test_getattr();
  printf("PASS\n");
  exit(0);
}
