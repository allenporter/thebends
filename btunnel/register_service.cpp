// register_service.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Command line tool that registers a service and blocks until ^C is pressed.

#include "CoreFoundation/CoreFoundation.h"
#include <err.h>
#include <sys/types.h>
#include <iostream>
#include "registration.h"
#include "google/gflags.h"
#include "ythread/mutex.h"
#include "ythread/condvar.h"

DEFINE_string(service_type, "_daap._tcp",
              "");
DEFINE_int32(service_port, 0,
             "");

using namespace std;

static ythread::Mutex mutex_;
static ythread::CondVar condvar_(&mutex_);

void sig_handler(int signal) {
  assert(signal = SIGINT);
  cout << "^C" << endl;
  mutex_.Lock();
  condvar_.Signal();
  mutex_.Unlock();
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_service_port <= 0) {
    errx(1, "Required flag --service_port was missing");
  }

  signal(SIGINT, sig_handler);
  uint16_t service_port = FLAGS_service_port;

  btunnel::RegisteredService* service =
    btunnel::NewRegisteredService(FLAGS_service_type, service_port);
  if (service == NULL) {
    errx(1, "Failed to create RegisteredService");
  }
  cout << "Service registered: " << service->name() << ", " << service->type()
       << ", " << service->domain() << " (" << service->port() << ")" << endl;

  mutex_.Lock();
  condvar_.Wait();
  mutex_.Unlock();

  cout << "Unregistering service" << endl;
  delete service;
}
