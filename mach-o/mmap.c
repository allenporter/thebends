/*
 * mmap.c
 */
#include "mmap.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <mach/vm_param.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * Open a file, memory map its contents, and populate mmap_info. Requires a
 * filename.
 */
int mmap_file_open(struct mmap_info *file_info) {
  assert(file_info != NULL);
  file_info->fd = open(file_info->name, O_RDWR, DEFFILEMODE);
  if (file_info->fd < 0) {
    perror("open");
    return -1;
  }
  struct stat sb;
  if (fstat(file_info->fd, &sb) < 0) {
    perror("fstat");
    return -1;
  }
  file_info->data_size = (size_t)sb.st_size;
  size_t map_size = file_info->data_size;
  file_info->data = mmap(0, map_size,
                         PROT_READ | PROT_WRITE,
                         MAP_FILE | MAP_SHARED,
                         file_info->fd, /* offset */0);
  if ((int)file_info->data < 0) {
    perror("mmap");
    return -1;
  }
  return 0;
}

/*
 * Unmemory map and close the file in file_info.
 */
int munmap_file(struct mmap_info *file_info) {
  if (munmap(file_info->data, file_info->data_size) < 0) {
    perror("munmap");
    return -1;
  }
  if (close(file_info->fd) < 0) {
    perror("close");
    return -1;
  }
  return 0;
}
