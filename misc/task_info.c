/*
 * Allen Porter <allen@thebends.org>
 * Dumps mach task info about a unix process.  For more info on the subject see
 * "Mac OS X Internals" by Amit Singh, and also check out
 * "Abusing Mach on Mac OS X" by nemo on uninformed.org.
 *
 * $ mig -v /usr/include/mach/task.defs
 * $ gcc -o task_info task_info.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_traps.h>
#include <mach/mach_types.h>
#include "task.h"  // generated from mach/tasks.def

int main (int argc, char* argv[]) {
  kern_return_t kr;
  task_t task;
  pid_t pid;
  if (argc != 2) {
    printf("usage: %s <pid>\n", argv[0]);
    return 1;
  }
  pid = atoi(argv[1]);
  if ((kr = task_for_pid(mach_task_self(), pid, &task)) != KERN_SUCCESS) {
    mach_error("task_for_pid", kr);
    return 1;
  }

  task_basic_info_data_t basic_info;
  mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
  if ((kr = task_info(task, TASK_BASIC_INFO,
                      (task_info_t)&basic_info, &count)) != KERN_SUCCESS) {
    mach_error("task_info", kr);
    exit(1);
  }
  if (count != TASK_BASIC_INFO_COUNT) {
    fprintf(stderr, "size mismatch");
    exit(1);
  }
  printf("-----------------------------------------------------------------\n");
  printf("Suspend count: %d\n", basic_info.suspend_count);
  printf("VM Size: 0x%08x\n", basic_info.virtual_size);
  printf("Resident memory: 0x%08x\n", basic_info.resident_size);
  printf("Default thread policy: ");
  switch (basic_info.policy) {
    case THREAD_STANDARD_POLICY:
      printf("Standard\n");
      break;
    case THREAD_TIME_CONSTRAINT_POLICY:
      printf("Time Constraint\n");
      break;
    case THREAD_PRECEDENCE_POLICY:
      printf("Precendence\n");
      break;
    default:
      printf("Unknown\n");
      break;
  } 
  printf("-----------------------------------------------------------------\n");
  task_events_info_data_t events_info;
  if ((kr = task_info(task, TASK_EVENTS_INFO,
                      (task_info_t)&events_info, &count)) != KERN_SUCCESS) {
    mach_error("task_info", kr);
    exit(1);
  }
  if (count != TASK_EVENTS_INFO_COUNT) {
    fprintf(stderr, "size mismatch");
    exit(1);
  }
  printf("Page faults: %d\n", events_info.faults);
  printf("Pageins: %d\n", events_info.pageins);
  printf("Copy-on-write faults: %d\n", events_info.cow_faults);
  printf("Messages sent: %d\n", events_info.messages_sent);
  printf("Messages received: %d\n", events_info.messages_received);
  printf("Mach system calls: %d\n", events_info.syscalls_mach);
  printf("Unix system calls: %d\n", events_info.syscalls_unix);
  printf("Context switches: %d\n", events_info.csw);
  printf("-----------------------------------------------------------------\n");
  return 0;
}

