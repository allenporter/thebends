// stack_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <assert.h>
#include <stdio.h>
#include "stack.h"

int main(int argc, char* argv[]);

void test1();

void bt ()
{
  void **fp = (void **) __builtin_frame_address (0);
  void *saved_pc = __builtin_return_address (0);
  void *saved_fp = __builtin_frame_address (1);
  int depth = 0;


  printf ("[%d] pc == %p fp == %p\n", depth++, saved_pc, saved_fp);
  fp = (void**)saved_fp;
  while (fp != NULL)
    {
      saved_fp = *fp;
      fp = (void**)saved_fp;
      if (*fp == NULL)
        break;
      saved_pc = *(fp + 2);
      printf ("[%d] pc == %p fp == %p\n", depth++, saved_pc, saved_fp);
    }
}

void d() {
  bt();
  assert(!profile::IsBelowStackFrame((void*)&main, (void*)&test1));
}

void c() {
  d();
}

void test1() {
  c();
//  assert(!profile::IsBelowStackFrame((void*)&main, (void*)&test1));
}

void test2() {
  assert(profile::IsBelowStackFrame((void*)&test2, (void*)&main));
}

int main(int argc, char* argv[]) {
  test1();
  test2();
}
