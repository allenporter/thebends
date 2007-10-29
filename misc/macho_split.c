/*
 * Allen Porter <allen@thebends.org>
 * Split up a mach_kernel.prelink into a bunch of smaller binary parts.  Works
 * by looking for the appropriate MH_MAGIC headers.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <mach-o/loader.h>
#include <mach-o/swap.h>
#include <assert.h>
#include <unistd.h>
#include "mmap_info.h"

int make_chunk(const char* buf, size_t nbytes, int num, const char* dir) {
  char outfile[BUFSIZ];
  snprintf(outfile, BUFSIZ, "%s/prelink_%d.bin", dir, num);
  printf("Writing previous chunk (size=%ld) to %s\n", nbytes, outfile);
  
  int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    return -1;
  }
  int wrote = write(fd, buf, nbytes);
  if (wrote == -1) {
    perror("write");
    return -1;
  }
  assert(wrote == nbytes);
  if (close(fd) == -1) {
    perror("close");
    return -1;
  }
  return 0;
}

int split(struct mmap_info* binary, const char* out_dir) {
  struct mach_header* mh = (struct mach_header*)binary->data;
  if (mh->magic != MH_MAGIC && mh->magic != MH_CIGAM) {
    return -1;
  }
  int count = 0;
  char* start = (char*)mh;
  char* last = start;
  char* cur = last + sizeof(struct mach_header);
  char* end = start + binary->data_size;
  while (cur < end) {
    if (*(long*)cur == MH_MAGIC || *(long*)cur == MH_CIGAM) {
      printf("Start of chunk @ %08x\n", (cur - start));
      if (make_chunk(last, (cur - last - 1), count, out_dir) == -1) {
        return -1;
      }
      last = cur;
      count++;
    }
    cur += 4;
  }
  if (last != cur) {
    if (make_chunk(last, (cur - last), count, out_dir) == -1) {
      return -1;
    }
  }
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s <kernel> <output directory>\n", argv[0]);
    exit(1);
  }
  struct mmap_info binary;
  strncpy(binary.name, argv[1], BUFSIZ);
  if (mmap_file_read(&binary) < 0) {
    fprintf(stderr, "Unable to open binary file: %s\n", binary.name);
    exit(1);
  }
  if (split(&binary, argv[2]) == -1) {
    fprintf(stderr, "Unable to split %s\n", binary.name);
  }
  munmap_file(&binary);
  return 0;
}
