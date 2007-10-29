/*
 * Allen Porter <allen@thebends.org>
 *
 * Analyzes the space usage of a mach-o binary based on the segment and
 * section headders.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>
#include "mmap_info.h"

void usage(char *argv[]) {
  fprintf(stderr, "usage: %s <binary>\n"
                  "  binary - program to analyzes\n", argv[0]);
}

void dump_at_offset(u_char* start, int offset, int size) {
  u_char* dump_start = start + offset;
  int end = 32 > size ? size : 32;
  int i;
  for (i = 0; i < end; ++i) {
    printf(" %02x", dump_start[i]);
  }
  printf("\n");
}

void file_space(struct mmap_info *bin, int dump) {
  u_char *start = (u_char *)bin->data;
  u_char *ptr = start;
  struct mach_header *mh = (struct mach_header *)start;
  if (bin->data_size < sizeof(struct mach_header)) {
    fprintf(stderr, "Binary too small (1)\n");
    return;
  }
  if (mh->magic != MH_MAGIC) {
    fprintf(stderr, "Binary contains an invalid magic number\n");
    return; 
  }
  if (mh->filetype != MH_EXECUTE) {
    fprintf(stderr, "Only tested on MH_EXECUTE");
    return;
  }
  printf("0x%08x : 0x%08lx HEADER\n", 0,
         sizeof(struct mach_header) + mh->sizeofcmds);
         

  uint32_t next_segment = 0;
  ptr += sizeof(struct mach_header);
  int i;
  for (i = 0; i < mh->ncmds; i++) {
    struct load_command *lc = (struct load_command *)ptr;
    if (lc->cmd == LC_SEGMENT) {
      struct segment_command *sc = (struct segment_command *)ptr;
      printf("0x%08x : 0x%08x\t%s (fsize:%u, vmsize:%u)\n",
             sc->fileoff, sc->fileoff + sc->filesize, sc->segname,
             sc->filesize, sc->vmsize);
      // Each segment should start where the last segment left off
      assert(next_segment == sc->fileoff);
      uint32_t space = 0;
      struct section *s = (struct section *)(sc + 1);
      int j;
      for (j = 0; j < sc->nsects; j++) {
        if ((s->flags & S_ZEROFILL) != S_ZEROFILL) {
          // zerofill sections don't take up disk space
          printf("0x%08x\t%s:%s (size: %d 0x%02x)\n",
               s->offset, s->segname, s->sectname, s->size, s->size);
          if (dump) {
            dump_at_offset(bin->data, s->offset, 16);
          }
        }
        space += s->size;
        s++;
      }
      printf("-------------------------------------------------------------\n");
      next_segment += sc->filesize;
    }
    ptr += lc->cmdsize;
  }
  // All load sections should cover the whole file
  assert(next_segment == bin->data_size);
  printf("PASS\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage(argv);
    exit(1);
  }
  struct mmap_info binary;
  strncpy(binary.name, argv[1], BUFSIZ);
  if (mmap_file_read(&binary) < 0) {
    fprintf(stderr, "Unable to open binary file: %s\n", binary.name);
    exit(1);
  }
  file_space(&binary, (argc == 3));
  munmap_file(&binary);
  return 0;
}
