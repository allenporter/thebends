/*
 * Simple utililty methods for mmaping files.
 */
#ifndef __MMAP_INFO_H__
#define __MMAP_INFO_H__

#include "stdio.h"

struct mmap_info {
  char name[BUFSIZ];  /* filename */
  int fd;             /* file descriptor */
  size_t data_size;   /* size of file */
  void *data;         /* mmap'd contents of file */
};

int mmap_file_read(struct mmap_info *file_info);
int munmap_file(struct mmap_info *file_info);

#endif  /* __MMAP_INFO_H__ */
