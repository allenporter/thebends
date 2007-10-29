/*
 * Allen Porter <allen@thebends.org>
 * Quick mig example, based on example from Mac OSX Internals by Amit Singh.
 * $ mig -v zoo.h
 * $ gcc -o client client.c zooUser.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/mach.h>
#include <servers/bootstrap.h>
#include "zoo.h"

#define MIG_ZOO_SERVICE "MIG-zooservice"

int main(int argc, char* argv[]) {
  kern_return_t kr;
  mach_port_t server_port;
  int32_t total;

  if ((kr = bootstrap_look_up(bootstrap_port, MIG_ZOO_SERVICE,
                              &server_port)) != BOOTSTRAP_SUCCESS) {
    mach_error("bootstrap_look_up:", kr);
    exit(1);
  }

  if ((kr = add_two_numbers(server_port, 5, 7, &total)) != KERN_SUCCESS) {
    mach_error("sum:", kr);
  } else {
    printf("sum of 5 and 7 is %d\n", total);
  }
  mach_port_deallocate(mach_task_self(), server_port);
  exit(0);
}
