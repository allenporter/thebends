// pty-example.c
// Author: Allen Porter <allen@thebends.org>
#include <stdio.h>
#include <errno.h>
#include <util.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
  int fd;
  pid_t pid = forkpty(&fd, NULL, NULL, NULL);
  if (pid == -1) {
    perror("forkpty");
    return 1;
  } else if (pid == 0) {
    if (execlp("/bin/sh", "sh", (void*)0) == -1) {
      perror("execlp");
    }
    fprintf(stderr, "program exited.\n");
    return 1;
  }
  printf("Child process: %d\n", pid);
  printf("master fd: %d\n", fd);

  // Set non-blocking
  int flags;
  if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
    flags = 0;
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl");
    return 1;
  }

  const char* cmd = "ls -l /\n";
  if (write(fd, cmd, strlen(cmd)) == -1) {
    perror("write");
    return 1;
  }

  // TODO: do something more interesting, like read input from stdin and write
  // it to the spawned process
  char buf[255];
  while (1) {
    int nread = read(fd, buf, 254);
    if (nread == -1) {
      if (errno == EAGAIN) {
        // no, sleep and try again
        usleep(1000);
        continue;
      }
      perror("read");
      break;
    }
    int i;
    for (i = 0; i < nread; i++) {
      putchar(buf[i]);
    }
  }
  printf("Done\n");
  return 0;
}
