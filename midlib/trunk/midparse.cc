// simple midi file parser

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include "buffer.h"
#include "midi.h"

static int mmap_file(const char* filename, void** region,
                     unsigned long* file_size) {
  int fd;
  if ((fd = open(filename, O_RDONLY, DEFFILEMODE)) < 0) {
    perror("unable to open file");
    return -1;
  }
  struct stat sb;
  if (fstat(fd, &sb) < 0) {
    perror("unable to stat file");
    return -1;
  }
  if (sb.st_size <= 0) {
    fprintf(stderr, "Invalid file size: %u\n", (unsigned int)sb.st_size);
    return -1;
  }
  *file_size = sb.st_size;
  if ((*region = mmap(0, sb.st_size, PROT_READ, MAP_FILE, fd, 0)) < 0) {
    perror("error mapping file");
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    exit(1);
  }

  void* data;
  unsigned long size;
  if (mmap_file(argv[1], &data, &size) < 0) {
    perror(argv[1]);
    exit(1);
  }

  midi::Buffer buffer((char*)data, size);
  midi::Midi midifile;
  if (!midifile.ParseFrom(&buffer)) {
    printf("Unable to parse midi file\n");
    exit(1);
  }

  if (munmap(data, size) < 0) {
    perror("error unmapping file");
    exit(1);
  }

  return 0;
}
