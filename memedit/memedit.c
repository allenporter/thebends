// memedit.c
// Allen Porter <allen@thebends.org>

#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <malloc/malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>
#include "google/gflags.h"

DEFINE_int32(pid, 0, "Process id to examine (required)");
DEFINE_string(op, "list",
              "Operation can be one of list, put or get");

// Flags for --put and --get
DEFINE_int32(offset, 0, "The offset (can be in hex) to operate on");
DEFINE_int32(size, 0, "TODO");
DEFINE_string(file, "", "Input or output file (depending on --op)");

void do_get(malloc_zone_t* zone, task_t task, vm_offset_t offset,
            size_t size, const std::string& filename) {
  vm_offset_t local_address;
  mach_msg_type_number_t local_size = size;
  kern_return_t kr = mach_vm_read(task, offset, size, &local_address,
                                  &local_size);
  if (kr != KERN_SUCCESS) {
    mach_error("mach_vm_read: ", kr);
    exit(kr);
  }
  int fd;
  if ((fd = open(filename.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0755)) == -1) {
    err(EX_CANTCREAT, "open");
  }
  if (write(fd, (void*)local_address, local_size) == -1) {
    err(EX_IOERR, "write");
  }
  close(fd);
}

void do_put(malloc_zone_t* zone, task_t task, vm_offset_t offset,
            size_t size, const std::string& filename) {
  int fd;
  if ((fd = open(filename.c_str(), O_RDONLY)) == -1) {
    err(EX_CANTCREAT, "open");
  }
  unsigned char* data = (unsigned char*)malloc(size * sizeof(char));
  ssize_t nbytes = read(fd, data, size);
  if (nbytes == -1) {
    free(data);
    err(EX_IOERR, "read");
  } else if ((size_t)nbytes != size) {
    free(data);
    errx(EX_IOERR, "only read %d bytes; expected %d", nbytes, size);
  }
  kern_return_t kr = mach_vm_write(task, offset, (vm_offset_t)data, size);
  if (kr != KERN_SUCCESS) {
    mach_error("mach_vm_read: ", kr);
    free(data);
    exit(kr);
  }
  free(data);
}

// TODO: Add a mode that combines output for continuous blocks
void iterate_callback(task_t task, void* context, unsigned type_mask,
                      vm_range_t* ranges, unsigned range_count) {
  vm_range_t* r;
  vm_range_t* end;
  for (r = ranges, end = ranges + range_count; r < end; ++r) {
    printf("%8p %u\n", (void*)r->address, r->size);
  }
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (geteuid() != 0) {
    errx(EX_USAGE, "You must be root to run this program");
  }
  if (FLAGS_pid <= 0) {
    errx(EX_USAGE, "Required flag --pid was missing");
  }
  if (FLAGS_op != "list" && FLAGS_op != "get" && FLAGS_op != "put") {
    errx(EX_USAGE, "Flag --op must be one of 'list', 'get', or 'put'");
  }
  if (FLAGS_op == "get" || FLAGS_op == "put") {
    if (FLAGS_file.empty()) {
      errx(EX_USAGE, "Required flag --file was missing");
    } else if (FLAGS_size <= 0) {
      errx(EX_USAGE, "Required flag --size must be > 0");
    } else if (FLAGS_offset <= 0) {
      errx(EX_USAGE, "Required flag --offset must be > 0");
    }
  }
  if ((FLAGS_op == "get" || FLAGS_op == "put") &&
      FLAGS_file.empty()) {
  }
  malloc_zone_t* zone = malloc_default_zone();
  if (zone == NULL) {
    err(EX_OSERR, "malloc_default_zone");
    exit(1);
  }
  kern_return_t kr;
  task_t task;
  if ((kr = task_for_pid(mach_task_self(), FLAGS_pid, &task)) != KERN_SUCCESS) {
    mach_error("task_for_pid", kr);
    return 1;
  }
  if (FLAGS_op == "list") {
    zone->introspect->enumerator(task, NULL, MALLOC_PTR_IN_USE_RANGE_TYPE,
                                 (vm_address_t)zone, NULL, iterate_callback);
  } else if (FLAGS_op == "get") {
    do_get(zone, task, FLAGS_offset, FLAGS_size, FLAGS_file);
  } else {
    do_put(zone, task, FLAGS_offset, FLAGS_size, FLAGS_file);
  }
  return 0;
}
