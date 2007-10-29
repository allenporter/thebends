/*
 * Allen Porter <allen@thebends.org>
 *
 * Shows the space allocation of a segment, by its sections.
 */
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>
#include "mmap_info.h"

extern Boolean
kld_macho_swap(struct mach_header* mh);

void usage(char *argv[]) {
  fprintf(stderr, "usage: %s <binary> <segment>\n"
                  "  binary - program to examine\n"
                  "  segment - name of the section to load\n", argv[0]);
}

int sig(struct mmap_info *bin, const char* segment) {
  u_char *start = (u_char *)bin->data;
  u_char *ptr = start;

  struct mach_header *mh = (struct mach_header *)start;
  if (bin->data_size < sizeof(struct mach_header)) {
    fprintf(stderr, "Binary too small (1)\n");
    return -1;
  }
  kld_macho_swap(mh);
  if (mh->magic != MH_MAGIC) {
    fprintf(stderr, "Binary contains an invalid magic number\n");
    return -1; 
  }
  if (mh->filetype != MH_EXECUTE) {
    fprintf(stderr, "Only tested on MH_EXECUTE");
    return -1;
  }
  ptr += sizeof(struct mach_header);

  uint32_t next_addr = 0;
  int i;
  for (i = 0; i < mh->ncmds; i++) {
    struct load_command *lc = (struct load_command *)ptr;
    if (lc->cmd == LC_SEGMENT) {
      struct segment_command *sc = (struct segment_command *)ptr;
      assert(sc->cmdsize ==
             (sizeof(struct segment_command) +
              sizeof(struct section) * sc->nsects));
      if (strncmp(sc->segname, segment, 16) == 0) {
        printf("SEG:%s\n", sc->segname);
        printf("VM: 0x%08x to 0x%08x)\n", sc->vmaddr, sc->vmsize + sc->vmaddr);
        int j;
        struct section *s = (struct section *)(sc + 1);
        next_addr = sc->vmaddr;
        uint32_t filled = 0;
        for (j = 0; j < sc->nsects; j++) {
          printf("SECT:%s; RANGE: 0x%08x to 0x%08x (%0.2f%%)\n",
                 s->sectname, s->addr, s->addr + s->size,
                 100.0 * s->size / sc->vmsize);
          filled += s->size;
          assert(s->addr >= next_addr);
          next_addr += s->size;
          s++;
        }
        printf("Unallocated: %u\n", sc->vmsize - filled);
      }
    }
    ptr += lc->cmdsize;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    usage(argv);
    exit(1);
  }
  struct mmap_info binary;
  strncpy(binary.name, argv[1], BUFSIZ);
  if (mmap_file_read(&binary) < 0) {
    fprintf(stderr, "Unable to open binary file: %s\n", binary.name);
    exit(1);
  }
  sig(&binary, argv[2]);
  munmap_file(&binary);
  return 0;
}
