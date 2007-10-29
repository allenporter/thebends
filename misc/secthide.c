/*
 * Allen Porter <allen@thebends.org>
 * Stuff arbitrary data in the __DATA __dyld section of a file.
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <mach/mach_init.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>
#include <unistd.h>
#include "mmap_info.h"

void usage(char *argv[]) {
  fprintf(stderr, "usage: %s <binary> <file>\n"
                  "  binary - program to hide data in\n"
                  "  file   - file containing data to hide\n", argv[0]);
}

/*
 * Adjusts a size x based on alignment y
 */
int adjust_size(x, y) {
  if (x % y == 0)
    return x;
  return (x + (y - (x % y)));
}

/*
 * Re-writes the VM addresses of all segments and sections
 */
void adjust_vm_alignment(struct mach_header *mh) {
  u_char *ptr = (u_char*)mh + sizeof(struct mach_header);
  uint32_t vmaddr = 0;
  int i;
  for (i = 0; i < mh->ncmds; i++) {
    struct load_command *lc = (struct load_command *)ptr;
    if (lc->cmd == LC_SEGMENT) {
      struct segment_command *sc = (struct segment_command *)ptr;
      // skip text segment ; probably should be more robust and skip only the
      // expected areas.
      if (strncmp(sc->segname, SEG_TEXT, 16) != 0) {
        printf("segname=%s @ 0x%x\n", sc->segname, vmaddr);
        sc->vmaddr = vmaddr;
        uint32_t orig_size = sc->vmsize;
        sc->vmsize = 0;
        struct section *s = (struct section *)(sc + 1);
        struct section *end_section = s;
        end_section += sc->nsects;
        for (; s < end_section; ++s) {
          printf(".section=%s\n", s->sectname);
          s->addr = vmaddr;
          s->size = adjust_size(s->size, 1 << s->align);
          sc->vmsize += s->size;
          vmaddr += s->size;
        }
        printf("vmsize=%d => ", sc->vmsize);
        sc->vmsize = adjust_size(sc->vmsize, vm_page_size);
        printf("vmsize=%d\n", sc->vmsize);
        if (sc->vmsize < orig_size) {
          sc->vmsize = orig_size;
        }
      }
      vmaddr = sc->vmaddr + sc->vmsize;
    }
    ptr += lc->cmdsize;
  }
}

#define SHIFT(name, structure, field, offset, shifty) \
  assert(offset != 0); \
  if (structure->field >= offset && structure->field > 0) { \
    printf("Shifting %s->%s by %d\n", #name, #field, shifty); \
    structure->field += shifty; \
  } else { \
    printf("Not shifting %s->%s value=%d (offset=%d, shift=%d)\n", \
           #name, #field, structure->field, offset, shifty); \
  }

/*
 * Stuff bin with data in hide
 */
int stuff(struct mmap_info *bin, struct mmap_info *hide,
          struct mmap_info *bin_out) {
  u_char *start = (u_char *)bin->data;
  u_char *ptr = start;
  u_char *ptr_out = (u_char *)bin_out->data;

  struct mach_header *mh = (struct mach_header *)start;
  if (bin->data_size < sizeof(struct mach_header)) {
    fprintf(stderr, "Binary too small (1)\n");
    return -1;
  }
  if (mh->magic != MH_MAGIC) {
    fprintf(stderr, "Binary contains an invalid magic number\n");
    return -1; 
  }
  ptr += sizeof(struct mach_header);

  // TODO(aporter): Use zero'd empty space instead of new space
  int i;
  uint32_t shift = 0;
  uint32_t insert_offset = 0;
  for (i = 0; i < mh->ncmds; i++) {
    struct load_command *lc = (struct load_command *)ptr;
    if (lc->cmd == LC_SEGMENT) {
      struct segment_command *sc = (struct segment_command *)ptr;
      if (sc->filesize > 0) {
        printf("Copying %s (%d bytes @ 0x%08x [orig offset: 0x%08x]) "
               "shift=%d\n",
               sc->segname, sc->filesize, ptr_out - (u_char*)bin_out->data,
               sc->fileoff, shift);
        memcpy(ptr_out, start + sc->fileoff, sc->filesize);
        if (shift > 0) {
          sc->fileoff = ptr_out - (u_char*)bin_out->data; //+= shift;
        }
        assert(sc->fileoff == (ptr_out - (u_char*)bin_out->data));
        ptr_out += sc->filesize;
        assert((ptr_out - (u_char*)bin_out->data) <= bin_out->data_size);
      }
      if (strncmp(sc->segname, SEG_DATA, 16) == 0) {
        insert_offset = sc->fileoff + sc->filesize;
        printf("Inserting %lu bytes @ 0x%08x\n",
               hide->data_size, (ptr_out - (u_char*)bin_out->data));
        uint32_t fill_size = adjust_size(hide->data_size, vm_page_size);
        sc->filesize += fill_size;
        sc->vmsize += fill_size;

        memcpy(ptr_out, hide->data, hide->data_size);
        ptr_out += hide->data_size;

        printf("Inserting %lu bytes of zeros @ 0x%08x\n",
               fill_size - hide->data_size,
               (ptr_out - (u_char*)bin_out->data));
        bzero(ptr_out, fill_size - hide->data_size);
        ptr_out += fill_size - hide->data_size;
        shift = fill_size;
        printf("Offset @ 0x%08x\n", (ptr_out - (u_char*)bin_out->data));
      }
    } else if (lc->cmd == LC_SYMTAB) {
      printf("LC_SYMTAB\n");
      struct symtab_command *sc = (struct symtab_command*)ptr;
      SHIFT(LC_SYMTAB, sc, symoff, insert_offset, shift);
      SHIFT(LC_SYMTAB, sc, stroff, insert_offset, shift);
    } else if (lc->cmd == LC_DYSYMTAB) {
      printf("LC_DYSYMTAB\n");
      struct dysymtab_command *dc = (struct dysymtab_command*)ptr;
      SHIFT(LC_DYSYMTAB, dc, tocoff, insert_offset, shift);
      SHIFT(LC_DYSYMTAB, dc, modtaboff, insert_offset, shift);
      SHIFT(LC_DYSYMTAB, dc, indirectsymoff, insert_offset, shift);
      SHIFT(LC_DYSYMTAB, dc, extrefsymoff, insert_offset, shift);
      SHIFT(LC_DYSYMTAB, dc, extreloff, insert_offset, shift);
      SHIFT(LC_DYSYMTAB, dc, locreloff, insert_offset, shift);
    } else if (lc->cmd == LC_TWOLEVEL_HINTS) {
      printf("LC_TWOLEVEL_HINTS\n");
      struct twolevel_hints_command *tc = (struct twolevel_hints_command*)ptr;
      SHIFT(LC_TWOLEVEL_HINTS, tc, offset, insert_offset, shift);
    }
    ptr += lc->cmdsize;
  }
  printf("\n");
  uint32_t written = ptr_out - (u_char*)bin_out->data;
  assert(written == bin_out->data_size);

  // re-write header
  adjust_vm_alignment(mh);
  memcpy(bin_out->data, bin->data, sizeof(struct mach_header) + mh->sizeofcmds);
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

  struct mmap_info hide;
  strncpy(hide.name, argv[2], BUFSIZ);
  if (mmap_file_read(&hide) < 0) {
    fprintf(stderr, "Unable to open hide file: %s\n", hide.name);
    exit(1);
  }

  struct mmap_info binary_out;
  snprintf(binary_out.name, BUFSIZ, "%s.new", binary.name);
  binary_out.data_size = binary.data_size +
                         adjust_size(hide.data_size, vm_page_size);
  binary_out.data = malloc(sizeof(u_char) * binary_out.data_size);
  bzero(binary_out.data, binary_out.data_size);

  if (stuff(&binary, &hide, &binary_out) < 0) {
    fprintf(stderr, "Unable to stuff binary\n");
    exit(1);
  }

  munmap_file(&binary);
  munmap_file(&hide);

  if ((binary_out.fd = open(binary_out.name, O_WRONLY | O_CREAT | O_TRUNC,
                             DEFFILEMODE)) < 0) {
    perror("open");
    fprintf(stderr, "Unable to open %s\n", binary_out.name);
    return 1;
  }
  if (write(binary_out.fd, binary_out.data, binary_out.data_size) < 0) {
    perror("write");
    fprintf(stderr, "Unable to write %s\n", binary_out.name);
    return 1;
  }

  close(binary_out.fd);
  free(binary_out.data);
  printf("PASS\n");
  return 0;
}
