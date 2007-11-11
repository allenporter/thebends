// Tutoral #3
// Author: Allen Porter <allen@thebends.org>
//
// Compile with:
// $ gcc -o wtmp_example wtmp_example.c
//
// 1. Run the program, and verify that the number of groups matches the output
//    of the unix command "id"
// 2. Modify this program so that it compiles with compiler warnings enabled
//    by adding flags -Wall -Werror.
// 3. Change the first argument of the getgroups() call from NGROUPS_MAX to -1
//    and re-run the program.  It should fail with an error message provided
//    by perror.  Re-write the error handing routine so that it doesn't use
//    perror(), but produces equivalent error output using strerror().  (Note:
//    perror() outputs to stderr by default, and you should make your error
//    function do the same).  See: man perror, man intro.
//    Change the -1 back to NGROUPS_MAX when finished, so the program works
//    again.
// 4. Print out the current userid.  See man getuid
// 5. Print out all of the group ids that the current user is a member of,
//    which is returned in the groups array.
// 6. Make this program output look exaclty the same as the unix command "id",
//    using these functions:  getuid, getgid, getgroups, getgrgid.  The man
//    pages for each of those functions will be helpful.

#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  gid_t groups[NGROUPS_MAX];
  int num_groups = getgroups(NGROUPS_MAX, groups);
  if (num_groups == -1) {
    perror("getgroups");
    return;
  }
  printf("You are in %d unix groups\n", num_groups);
}
