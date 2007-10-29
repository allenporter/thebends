/*
 * reboot.c
 * Author: Allen Porter <allen@thebends.org>
 *
 * Compile for iphone:
 * $ arm-apple-darwin-cc -Wall -o reboot reboot.c
 */

#include <unistd.h>
#include <sys/reboot.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  printf("rebooting... please wait.");
  return reboot(RB_AUTOBOOT);
}
