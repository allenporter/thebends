/*
 * Allen Porter <allen@thebends.org>
 *
 * Create a really basic signature of a mach-o file.  The general idea is to
 * parse the mach-o header files and output a little bit of data that the
 * each section points to.  Useful if you make modifications to a mach-o binary
 * and want to see if all of the pointers are still intact.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>
#include "mmap_info.h"

void usage(char *argv[]) {
  fprintf(stderr, "usage: %s <binary>\n"
                  "  binary - program to get a signature for\n", argv[0]);
}

void dump_at_offset(u_char* start, int offset, int size) {
  u_char* dump_start = start + offset;
  int end = 16 > size ? size : 16;
  int i;
  for (i = 0; i < end; ++i) {
    printf(" %x", dump_start[i]);
  }
  printf("\n");
}

int sig(struct mmap_info *bin) {
  u_char *start = (u_char *)bin->data;
  u_char *ptr = start;

  struct mach_header *mh = (struct mach_header *)start;
  if (bin->data_size < sizeof(struct mach_header)) {
    fprintf(stderr, "Binary too small (1)\n");
    return -1;
  }
  if (mh->magic != MH_MAGIC) {
    fprintf(stderr, "Binary contains an invalid magic number\n");
    return -1; 
  }
  if (mh->filetype != MH_EXECUTE) {
    fprintf(stderr, "Only tested on MH_EXECUTE");
    return -1;
  }
  ptr += sizeof(struct mach_header);

  int i;
  for (i = 0; i < mh->ncmds; i++) {
    struct load_command *lc = (struct load_command *)ptr;
    if (lc->cmd == LC_SEGMENT) {
      struct segment_command *sc = (struct segment_command *)ptr;
      assert(sc->cmdsize ==
             (sizeof(struct segment_command) +
              sizeof(struct section) * sc->nsects));
      printf("SEG:%s:", sc->segname);
      dump_at_offset(start, sc->fileoff, sc->filesize);

      int j;
      struct section *s = (struct section *)(sc + 1);
      for (j = 0; j < sc->nsects; j++) {
        printf("SECT:%s:%s:", s->segname, s->sectname);
        dump_at_offset(start, s->offset, s->size);
        s++;
      }
    } else if (lc->cmd == LC_DYSYMTAB) {
      struct dysymtab_command *dc = (struct dysymtab_command*)ptr;
      printf("DYSYMTAB:tocoff");
      dump_at_offset(start, dc->tocoff,
                     dc->ntoc * sizeof(struct dylib_table_of_contents));
      printf("DYSYMTAB:modtaboff");
      dump_at_offset(start, dc->modtaboff,
                     dc->nmodtab * sizeof(struct dylib_module));
      printf("DYSYMTAB:indirectsymoff");
      dump_at_offset(start, dc->indirectsymoff,
                     dc->nindirectsyms * sizeof(struct dylib_reference));
      printf("DYSYMTAB:extrefsymoff");
      dump_at_offset(start, dc->extrefsymoff,
                     dc->nextrefsyms * sizeof(struct dylib_reference));
      printf("DYSYMTAB:extreloff");
      dump_at_offset(start, dc->extreloff,
                     dc->nextrel * sizeof(struct relocation_info));
      printf("DYSYMTAB:locreloff");
      dump_at_offset(start, dc->locreloff,
                     dc->nlocrel * sizeof(struct relocation_info));
    } else if (lc->cmd == LC_SYMTAB) {
      struct symtab_command *sc = (struct symtab_command*)ptr;
      printf("SYMTAB:symoff");
      dump_at_offset(start, sc->symoff, sc->nsyms * sizeof(struct nlist));
      printf("SYMTAB:stroff");
      dump_at_offset(start, sc->stroff, sc->strsize);
    } else if (lc->cmd == LC_TWOLEVEL_HINTS) {
      struct twolevel_hints_command *tc = (struct twolevel_hints_command*)ptr;
      printf("LC_TWOLEVEL_HINTS:offset");
      dump_at_offset(start, tc->offset,
                     tc->nhints * sizeof(struct twolevel_hint));
    } else if (lc->cmd == LC_LOAD_DYLINKER ||
               lc->cmd == LC_LOAD_DYLIB ||
               lc->cmd == LC_UNIXTHREAD) {
      // skip
    } else {
      printf("Unrecognized load command: %x\n", lc->cmd);
      exit(1);
    }
    ptr += lc->cmdsize;
  }
  return 0;
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
  sig(&binary);
  munmap_file(&binary);
  return 0;
}
