/*
 * Allen Porter <allen@thebends.org>
 * Extract extensions from an uncompressed mach_kernel.prelink.
 * $ gcc -Wall -o prelink_extract prelink_extract.c mmap_info.c \
 *       -framework IOKit -framework CoreFoundation
 */
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mach-o/loader.h>
#include <mach-o/swap.h>
#include "mmap_info.h"

#include <IOKit/IOCFUnserialize.h>

struct prelink_header {
  struct segment_command segment;
  struct section section[1];
};

extern Boolean
kld_macho_swap(struct mach_header* mh);

int get_section(struct mach_header* mh, const char* segment,
                const char* section, struct section**return_section) {
  u_char* ptr = (u_char*)(mh + 1);
  int i;
  for (i = 0; i < mh->ncmds; ++i) {
    struct load_command* lc = (struct load_command*)ptr;
    if (lc->cmd == LC_SEGMENT) {
      struct segment_command* sc = (struct segment_command*)ptr;
      assert(sc->cmdsize == (sizeof(struct segment_command) +
                             sizeof(struct section) * sc->nsects));
      if (strcmp(segment, sc->segname) == 0) {
        struct section *s = (struct section *)(sc + 1);
        struct section *end_section = s + sc->nsects;
        for (; s < end_section; ++s) {
          if (strcmp(section, s->sectname) == 0) {
            *return_section = s;
            return 0;
          }
        }
      }
    } 
    ptr += lc->cmdsize;
  }
  fprintf(stderr, "Segment + section not found");
  return -1;
}

int get_segment(struct mach_header* mh, const char* segment,
                struct segment_command** return_segment) {
  u_char* ptr = (u_char*)(mh + 1);
  int i;
  for (i = 0; i < mh->ncmds; ++i) {
    struct load_command* lc = (struct load_command*)ptr;
    if (lc->cmd == LC_SEGMENT) {
      struct segment_command* sc = (struct segment_command*)ptr;
      assert(sc->cmdsize == (sizeof(struct segment_command) +
                             sizeof(struct section) * sc->nsects));
      if (strcmp(segment, sc->segname) == 0) {
        *return_segment = sc;
        return 0;
      }
    } 
    ptr += lc->cmdsize;
  }
  fprintf(stderr, "Segment not found");
  return -1;
}

void WritePropertyListToFile(CFPropertyListRef propertyList,
                             CFURLRef fileURL ) {
   CFDataRef xmlData;
   Boolean status;
   // Convert the property list into XML data.
   xmlData = CFPropertyListCreateXMLData( kCFAllocatorDefault, propertyList );
   // Write the XML data to the file.
   status = CFURLWriteDataAndPropertiesToResource(fileURL, xmlData, NULL, NULL);
   CFRelease(xmlData);
}

int extract_module(struct mach_header* mh) {
  assert(mh->magic == MH_MAGIC || mh->magic == MH_CIGAM);
  kld_macho_swap(mh);
  assert(mh->magic == MH_MAGIC);
  char* ptr = (char*)(mh + 1);
  int i;
  for (i = 0; i < mh->ncmds; ++i) {
    struct load_command* lc = (struct load_command*)ptr;
    switch (lc->cmd) {
      case LC_SEGMENT: {
        struct segment_command* sc = (struct segment_command*)lc;
        printf("fileoff=%d\n", sc->fileoff);
        printf("filesize=%d\n", sc->filesize);
        break;
      }
      case LC_SYMTAB: {
        struct symtab_command* sc = (struct symtab_command*)lc;
        printf("symoff=%d\n", sc->symoff);
        printf("stroff=%d\n", sc->strsize);
        break;
      }
      default:
        printf("Die! %d\n", lc->cmd);
        return -1;
    }
    ptr += lc->cmdsize;
  }
  return 0;
}

int extract_prelink(struct mach_header* mh, struct prelink_header* prelink,
                    const char* output_dir) {
  struct section* text = &prelink->section[0];
  char* ptr = (char*)mh + text->offset;
/*
  while (1) { 
    struct mach_header* module = (struct mach_header*)ptr;
    printf("Module at %lx\n", (long)(module - mh));
    extract_module(module);
    char *c = NULL;
    *c = '1';
  }
*/
  while (1) {
    struct mach_header* module = (struct mach_header*)ptr;
    if (module->magic == MH_MAGIC || module->magic == MH_CIGAM) {
      printf("Found at %lx\n", (long)(ptr - (char*)mh));
    }
    ptr += 1;
  }

/*
  struct section* s = &prelink->section[1];
  char* ptr = (char*)mh + s->offset;
  struct symtab_command* symtab = (struct symtab_command*)ptr;
  ptr += symtab->symoff;
  int i;
  for (i = 0; i < symtab->nsyms; i++) {
    struct nlist* list = (struct nlist*)ptr;
    printf("type=%d\n", list->n_type);
    printf("sect=%d\n", list->n_sect);
    printf("desc=%d\n", list->n_desc);
    printf("value=%d\n", list->n_value);
    ptr += sizeof(struct nlist);
  }
*/



/*
  struct section* info = &prelink->section[2];
  char* ptr = (char*)mh + info->offset;
  CFStringRef error = NULL;
  CFArrayRef dicts;
  if ((dicts = IOCFUnserialize(ptr, kCFAllocatorDefault, 0, &error)) == 0) {
    CFShow(error);
    CFRelease(error);
    return -1;
  }
  CFIndex i;
  for (i = 0; i < CFArrayGetCount(dicts); ++i) {
    // TODO(allen): Chop out the extension itself
    CFDictionaryRef dict = CFArrayGetValueAtIndex(dicts, i);;
    CFStringRef id = CFDictionaryGetValue(dict, CFSTR("CFBundleIdentifier"));
    CFStringRef path = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
                                                CFSTR("%s/%@/Info.plist"),
                                                output_dir, id);
    CFShow(path);
    CFURLRef fileURL;
    fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, path,
                                            kCFURLPOSIXPathStyle, false);
    WritePropertyListToFile(dict, fileURL);
    CFDataRef data = CFDictionaryGetValue(dict, CFSTR("OSBundlePrelink"));
    if (data) {
      UInt32 prelinkInfo[4];
      CFRange all;
      all.location = 0;
      all.length = CFDataGetLength(data);
      CFDataGetBytes(data, all, (UInt8*)prelinkInfo);
      printf("%u - %ld - %ld - %ld\n", NXSwapHostIntToBig(prelinkInfo[0]), prelinkInfo[1],
             prelinkInfo[2], prelinkInfo[3]);
    }
    CFRelease(path);
  }
*/
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s <kernel> <output directory>\n", argv[0]);
    exit(1);
  }
  struct mmap_info binary;
  strncpy(binary.name, argv[1], BUFSIZ);
  if (mmap_file_read(&binary) < 0) {
    fprintf(stderr, "Unable to open binary file: %s\n", binary.name);
    exit(1);
  }
  struct mach_header* mh = (struct mach_header*)binary.data;
  assert(mh->magic == MH_MAGIC || mh->magic == MH_CIGAM);
  kld_macho_swap(mh);
  assert(mh->magic == MH_MAGIC);

  struct prelink_header* prelink;
  if (get_segment(mh, "__PRELINK", (struct segment_command**)&prelink) == -1) {
    fprintf(stderr, "Unable to find __PRELINK segment in %s\n", binary.name);
  } else {
    assert(strcmp("__text",   prelink->section[0].sectname) == 0 &&
           strcmp("__symtab", prelink->section[1].sectname) == 0 &&
           strcmp("__info",   prelink->section[2].sectname) == 0);
    if (extract_prelink(mh, prelink, argv[2]) == -1) {
      fprintf(stderr, "Unable to extract extensions from: %s\n", binary.name);
    }
  }
  munmap_file(&binary);
  return 0;
}
