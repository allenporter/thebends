#include "stack.h"

#include <assert.h>
#include <stdio.h> // TODO(aporter) REMOVE
#include <dlfcn.h>

namespace profile {

static void PrintFrame(void *fp, void *ra) {
  int ret;
  Dl_info info;
  ret = dladdr(ra, &info);
  printf("%s%s in %s, fp = %p, pc = %p\n",
         (ret) ? info.dli_sname : "?",
         (ret) ? "()" : "",
         (ret) ? info.dli_fname : "?", fp, ra);
}

// TODO(aporter): This doesn't seem to work
bool IsBelowStackFrame(void *start_address, void *address) {
  void *ra = __builtin_return_address(0);
  void **fp = (void**)__builtin_frame_address(0);
  void *saved_fp = __builtin_frame_address(1);
  PrintFrame(saved_fp, ra);
  fp = (void**)saved_fp;
//  bool found_start = false;
  while (fp) {
    saved_fp = *fp;
    fp = (void**)saved_fp;
    if (*fp == NULL) {
      break;
    }
    ra = *(fp + 2);
    PrintFrame(saved_fp, ra);
/*
    if (!found_start && start_address == fp) {
printf("found\n");
      found_start = true;
    } else if (found_start && address == fp) {
printf("found 2\n");
      return true;
    }
*/
  }
  printf("zzz\n");
  return false;
}

}  // namespace profile
