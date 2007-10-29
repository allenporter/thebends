// png-check.c
// Author: Allen Porter <allen@thebends.org>
// See RFC-2083 for details about PNG and RFC-1950 for details about deflate
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

const unsigned char signature[8] = "\211PNG\r\n\032\n";

// verifies the PNG signature and moves the pointer past it
int verify(unsigned char** buf) {
  unsigned char* data = (*buf);
  int ret = 1;
  int i;
  for (i = 0; i < 8; ++i) {
    if (data[i] != signature[i]) {
      fprintf(stderr, "sig[%d]: %x != %x\n", i, data[i], signature[i]);
      ret = 0;
    }
  }
  // advance cursor
  *buf = &data[8];
  return ret;
}

void dump_ihdr(unsigned char* buf, int length) {
  struct ihdr {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression;
    uint8_t filter;
    uint8_t interlace;
  };
  struct ihdr* h = (struct ihdr*)buf;
  printf("  Width: %d\n", h->width);
  printf("  Height: %d\n", h->height);
  printf("  Bit Depth: %d\n", h->bit_depth);
  printf("  ColorType: %d\n", h->color_type);
  assert(h->compression == 0);  // (deflate/inflate is the only type supported)
  printf("  Filter method: %d\n", h->filter);
  printf("  Interlace: %d\n", h->interlace);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    errx(EX_USAGE, "Usage: %s <file>", argv[0]);
  }

  const char* filename = argv[1];
  printf("reading %s\n", filename);
  int fd;
  if ((fd = open(filename, O_RDONLY)) < 0) {
    err(EX_IOERR, "open");
  }
  struct stat sb;
  if (fstat(fd, &sb) < 0) {
    err(EX_IOERR, "stat");
  }

  assert(sb.st_size > 8);
  unsigned char* buf = malloc(sb.st_size);
  if (read(fd, buf, sb.st_size) != sb.st_size) {
    err(EX_IOERR, "read");
  }
  unsigned char* cur = buf;
  unsigned char* end = buf + sb.st_size;
  if (!verify(&cur)) {
    errx(EX_DATAERR, "PNG Signature mismatch");
    return 1;
  } else {
    printf("Signature verified\n");
  }

  while (cur < end) {
    printf("--------------------\n");
    printf("Offset: 0x%06x\n", cur - buf);
    uint32_t* chunk_length = (uint32_t*)cur;
    cur += 4;
    printf("Chunk length: %d\n", *chunk_length);
    char chunk_type[4];
    memcpy(chunk_type, cur, 4);
    printf("Chunk type: %c%c%c%c\n", chunk_type[0], chunk_type[1],
           chunk_type[2], chunk_type[3]);
    cur += 4;
    if (memcmp(chunk_type, "IHDR", 4) == 0) {
      dump_ihdr(cur, *chunk_length);
    } else if (*chunk_length > 0) {
      // dump raw chunk data that we wont bother parsing
      printf("Chunk data:\n ");
      int i;
      for (i = 0; i < *chunk_length; i++) {
        if (i != 0 && i % 14 == 0) {
          printf("\n ");
        }
        printf(" 0x%02x", cur[i]);
      }
      printf("\n");
    }
    cur += *chunk_length;
    // TODO: Verify CRC for all blocks
    printf("CRC: %02x %02x %02x %02x\n", cur[0], cur[1], cur[2], cur[3]);
    cur += 4;
  } 

  free(buf);
  return 0;
}
