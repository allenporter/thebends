// profile.cpp
// Author: Allen Porter <allen@thebends.org>
// TODO(allen): Work with threaded applications?

#include "profile.h"

#include <err.h>
#include <malloc/malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include <iostream>
#include <string>
#include <set>

using namespace std;

namespace profile {

static const char* kEnvDebug = "PROFILE_DEBUG";

//#define DLOG(s) if (config.debug) { malloc_printf(s); }

struct Config {
  Config() : debug(false)
  { }

  bool debug;
};

static Config config;
static malloc_zone_t original_zone;
static set<void*> pointers;
static int in_profile = false;

static void track(const char* label, void* ptr, size_t size) {
  if (in_profile) {
    return;
  }
  in_profile = true;
  if (config.debug) {
    malloc_printf("%s %p\n", label, ptr);
  }
  pointers.insert(ptr);
  in_profile = false;
}

static void untrack(const char* label, void* ptr) {
  if (in_profile) {
    return;
  }
  in_profile = true;
  if (config.debug) {
    malloc_printf("profile::free %p\n", ptr);
  }
  assert(pointers.count(ptr) == 1);
  pointers.erase(ptr);
  in_profile = false;
}

//
// Stub functions
//

void* profile_malloc(malloc_zone_t* zone, size_t size) {
  void* ptr = (*original_zone.malloc)(zone, size);
  track("profile::malloc", ptr, size);
  return ptr;
}

void* profile_calloc(malloc_zone_t* zone, size_t num_items, size_t size) {
  void* ptr = (*original_zone.calloc)(zone, num_items, size);
  track("profile::calloc", ptr, size);
  return ptr;
}

void* profile_valloc(malloc_zone_t* zone, size_t size) {
  void* ptr = (*original_zone.valloc)(zone, size);
  track("profile::valloc", ptr, size);
  return ptr;
}

void profile_free(malloc_zone_t* zone, void* ptr) {
  (*original_zone.free)(zone, ptr);
  untrack("profile::free", ptr);
}

static void Shutdown() {
  malloc_zone_t* zone = malloc_default_zone();
  if (zone == NULL) {
    err(EX_OSERR, "malloc_default_zone");
    exit(1);
  }
  *zone = original_zone;
  if (config.debug) {
    malloc_printf("profile::Shutdown\n");
  }
  if (pointers.size() > 0) {
    malloc_printf("profile::leaked %d items\n", pointers.size());
    exit(1);
  }
}

void Init() {
  atexit(&Shutdown);

  const char* debug = getenv(kEnvDebug);
  if (debug != NULL && strcmp(debug, "1") == 0) {
    config.debug = true;
  }

  malloc_zone_t* zone = malloc_default_zone();
  if (zone == NULL) {
    err(EX_OSERR, "malloc_default_zone");
    exit(1);
  }

  original_zone = *zone;
  zone->malloc = &profile_malloc;
  zone->calloc = &profile_calloc;
  zone->valloc = &profile_valloc;
  zone->free = &profile_free;
}

}  // namespace profile
