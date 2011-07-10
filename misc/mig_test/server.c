/*
 * Allen Porter <allen@thebends.org>
 * Quick mig example, based on example from Mac OSX Internals by Amit Singh.
 * $ mig -v zoo.defs
 * $ gcc -Wall -Werror -o server server.c zooServer.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/mach.h>
#include <servers/bootstrap.h>
#include "zoo.h"

static mach_port_t server_port;

#define MIG_ZOO_SERVICE "MIG-zooservice"

extern boolean_t zoo_server(
                mach_msg_header_t *InHeadP,
                mach_msg_header_t *OutHeadP);

void server_setup() {
  kern_return_t kr;
  if ((kr = bootstrap_check_in(bootstrap_port, MIG_ZOO_SERVICE,
                               &server_port)) != BOOTSTRAP_SUCCESS) {
    mach_port_deallocate(mach_task_self(), server_port);
    mach_error("bootstrap_check_in:", kr);
    exit(1);
  }
}

void server_loop() {
  mach_msg_server(zoo_server, sizeof(__Request__add_two_numbers_t),
                  server_port, MACH_MSG_TIMEOUT_NONE);
}

kern_return_t add_two_numbers(mach_port_t server_port, int32_t a, int32_t b,
                              int32_t* c) {
  printf("Incoming request: a=%d, b=%d\n", a, b);
  *c = a + b;
  printf("Result: %d\n", *c);
  return KERN_SUCCESS;
}

int main(int argc, char* argv[]) {
  server_setup();
  server_loop();
  return 0;
}
