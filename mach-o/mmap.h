/*
 * Simple utililty methods for mmaping files.
 */
#ifndef __MMAP_H__
#define __MMAP_H__

#include "sys/types.h"

#define MMAP_BUFSIZ 1024

struct mmap_info {
  char name[MMAP_BUFSIZ];  /* filename */
  int fd;                  /* file descriptor */
  size_t data_size;        /* size of file */
  void* data;              /* mmap'd writable contents of file */
};

/*
 * mmap (read/write) the specified file.
 */
int mmap_file_open(struct mmap_info *file_info);
int munmap_file(struct mmap_info *file_info);

#endif  /* __MMAP_H__ */
