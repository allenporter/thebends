/*
 * modify_eip.c
 * Author: Allen Porter <allen@thebends.org>
 *
 * Utility that modifies the instruction pointer to start at the specified
 * offset.  Assumes that the binary is properly formatted (ie, nobody is
 * trying to exploit this program).
 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <mach-o/swap.h>
#include <mach-o/i386/swap.h>
#include <mach/i386/thread_status.h>
#include "mmap.h"

/*
 * Contents of LC_UNIXTHREAD section for x86.
 */
struct thread_state {
  uint32_t flavor;
  uint32_t count;
  i386_thread_state_t cpu;  
};


/*
 * Return a pointer to the mach_header part of the binary.  This is typically
 * at the start of the file, but may be embedded in some fat headers.
 */
const struct mach_header* find_header(const void* data, size_t size) {
  if (size < sizeof(struct mach_header) || size < sizeof(struct fat_header)) {
    warn("File not large enough to contain a mach header");
    return NULL;
  }
  struct mach_header* mh = (struct mach_header*)data;
  if (mh->magic == MH_MAGIC || mh->magic == MH_CIGAM) {
    return mh;
  } else if (mh->magic == FAT_MAGIC || mh->magic == FAT_CIGAM) {
    struct fat_header* fh = (struct fat_header*)data;
    u_char *ptr = (u_char*)fh + sizeof(struct fat_header);
    long nfat_arch = NXSwapLong(fh->nfat_arch);
    int i;
    for (i = 0; i < nfat_arch; ++i) {
      struct fat_arch* fa = (struct fat_arch*)ptr;
      long offset = NXSwapLong(fa->offset);
      struct mach_header* mh = (struct mach_header*)(data + offset);
      if ((mh->magic == MH_MAGIC || mh->magic == MH_CIGAM) &&
          mh->cputype == CPU_TYPE_X86) {
        return mh;
      }
      ptr += sizeof(struct fat_arch);
    }
  } else {
    warnx("Unrecognized magic: 0x%x", mh->magic);
  }
  return NULL;
}

/*
 * Return a pointer to the LC_UNIXTHREAD command.
 */
const struct thread_command* find_thread_command(const struct mach_header* mh) {
  if (mh->magic != MH_MAGIC) {
    warnx("Incorrect MH_MAGIC in header: 0x%x", mh->magic);
    return NULL;
  }

  u_char *ptr = (u_char*)mh + sizeof(struct mach_header);
  int i;
  for (i = 0; i < mh->ncmds; i++) {
    struct load_command *lc = (struct load_command *)ptr;
    if (lc->cmd == LC_UNIXTHREAD) {
      struct thread_command *tc = (struct thread_command*)ptr;
      return tc;
    }
    ptr += lc->cmdsize;
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    errx(EX_USAGE, "Usage: %s <file> <hex offset>", argv[0]);
  }

  uint32_t new_eip;
  sscanf(argv[2], "%x", &new_eip);
  if (new_eip == 0) {
    errx(EX_USAGE, "Usage: %s <file> <hex offset>", argv[0]);
  }

  struct mmap_info info;
  strncpy(info.name, argv[1], MMAP_BUFSIZ);
  if (mmap_file_open(&info) != 0) {
    exit(EX_IOERR);
  }

  const struct mach_header* mh = find_header(info.data, info.data_size);
  if (mh == NULL) {
    errx(1, "No mach-o header found");
  }
  if (mh->cputype != CPU_TYPE_I386) {
    errx(1, "Only CPU_TYPE_I386 is supported: %d", mh->cputype);
  }

  const struct thread_command* tc = find_thread_command(mh);
  if (tc == NULL) {
    errx(1, "No LC_UNIXTHREAD command found");
  }
  if (tc->cmdsize < sizeof(struct thread_state)) {
    errx(1, "Invalid thread command size %d", tc->cmdsize);
  }

  struct thread_state* state = (struct thread_state*)(tc + 1);
  if (state->flavor != i386_THREAD_STATE) {
    errx(1, "Incorrect thread state: %d", state->flavor);
  }
  if (state->count != i386_THREAD_STATE_COUNT) {
    warn("Incorrect thread state count: %x", state->count);
  }
  printf("< eip: %08x\n", state->cpu.__eip);
  state->cpu.__eip = new_eip;
  printf("> eip: %08x\n", state->cpu.__eip);
  munmap_file(&info);
  return 0;
}
