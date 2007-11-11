// Compile with:
// $ gcc -Wall -Werror -o putc_example putc_example.c
//
// 1. Warm up: Make the program print out the string backwards
// 2. $ rm -f putc_example  (remove the compiled program)
//    Comment out all of the #include headers in this file and re-run the
//    compile command.  You should see warnings, and no ouput file is generated.
//    Try to get the program to compile/generate an output file without
//    adding the include files back in.  (There are two different ways that
//    I can think of to do this).
// 3. In C, arrays and pointers are the same thing.  Rewrite the "loop"
//    section below using pointers instead of arrays (ie, dont use the [ ]
//    syntax).
//    See: http://www.google.com/search?q=c+pointers+arrays


#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: %s <string>\n", argv[0]);
    return 0;
  }

  // This code is equivalent to printf("%s\n", argv[1]);
  const char* string_argument = argv[1];
  size_t length = strlen(string_argument);
  int i;
  for (i = 0; i < length; i++) {
    char c = string_argument[i];
    putchar(c);
  }
  putchar('\n');

  return 0;
}
