/*
 * append_segment.c
 * Author: Allen Porter <allen@thebends.org>
 *
 * Append arbitrary data to a mach-o binary.  The __PAGEZERO section is pointed
 * to the end of the file and extended using the technique described by roy g
 * biv.
 */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <unistd.h>

off_t filesize(int fd) {
  struct stat buf; 
  if (fstat(fd, &buf) < 0) {
    perror("fstat");
    return -1;
  }
  return buf.st_size;
}

int read_file(const char* const filename,
              char** outbuf,
              off_t* size) {
  int fd = open(filename, O_RDWR, DEFFILEMODE);
  if (fd < 0) {
    err(EX_IOERR, "open %s", filename);
    return -1;
  }

  off_t file_size = filesize(fd);
  if (file_size == -1) {
    return -1;
  }

  char* buf = (char*)malloc(file_size);
  ssize_t ret = read(fd, buf, file_size);
  if (ret < 0) {
    err(EX_IOERR, "read %s", filename);
    return -1;
  }
  if (ret != file_size) {
    err(EX_IOERR, "read got %d, expected %d", ret, file_size);
    return -1;
  } 
  close(fd);
  *outbuf = buf;
  *size = file_size;
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    err(EX_USAGE, "Usage: %s <binary file> <data file>");
  }

  char* data_buffer;
  off_t data_size;
  if (read_file(argv[1], &data_buffer, &data_size) != 0) {
    return 1;
  }

  // TODO(allen): Read in binary file
  // TODO(allen): Pad file size to multiple of 4k
  // TODO(allen): Append data_buffer
  // TODO(allen): ???
  // TODO(allen): Profit

  free(data_buffer);
}
