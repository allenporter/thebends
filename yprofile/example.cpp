// sample.cpp
// Author: Allen Porter <allen@thebends.org>

#include <iostream>
#include "profile.h"

using namespace std;

#define MALLOC 0

int main(int argc, char* argv) {
  profile::Init();

  if (MALLOC) {
    void* a = malloc(1024);
    void* b = malloc(1024);
    malloc(1024); // c

    free(a);
    free(b);
    // leak c!
  } else {
    char* a = new char;
    new char;  // b
    new char;  // c
    delete a;
    // leak b, c!
  }
}
