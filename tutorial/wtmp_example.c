// Tutorial #2: Show users that previous logged in/out of the system.
// Author: Allen Porter <allen@thebends.org>
//
// Compile with:
// $ gcc -o wtmp_example wtmp_example.c
//
// 1. Add to the output the tty the user logged in from, example:
//      Login/logout from user aporter on ttyp1 at 1194810372
//    See "man 5 utmp" for more info.
// 2. This program doesn't compile when you add the flags "-Wall -Werror", which
//    make the compiler halt on all warnings.  Modify this program so that
//    when compiling with those flags, it prints no warnings.
//    You may need to read the man page for the functions called in the file.
//    (Note that the man section number must be specified for some functions
//    that have equivalent shell commands such as printf. See man -f printf).
// 3. The output currently shows the unix time in seconds since 1/1/1970, which
//    isn't very useful.  Use the time() function, to display how many seconds
//    ago the user logged in.  For example:
//      Login/logout from user aporter on ttyp1, 250 second(s) ago.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <utmp.h>

int main(int argc, char* argv[]) {
  int fd = open(_PATH_WTMP, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Error opening file: %s", strerror(errno));
    return 1;
  }

  // Print all last logins
  do {
    struct utmp record;
    int nbytes = read(fd, &record, sizeof(struct utmp));
    if (nbytes == -1) {
      fprintf(stderr, "Error reading from file: %s", strerror(errno));
      break;
    } else if (nbytes == 0) {
      // End of file reached
      break;
    }
    printf("Login/logout from user %s at %d\n",
           record.ut_name,
           record.ut_time);
  } while (1);

  close(fd);

  return 0;
}
