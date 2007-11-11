// register_service.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Command line tool that registers a service and blocks until ^C is pressed.
// This tool is used for testing just the service registration module of
// btunnel.

#include "CoreFoundation/CoreFoundation.h"
#include <err.h>
#include <sys/types.h>
#include <iostream>
#include <google/gflags.h>
#include <ythread/mutex.h>
#include <ythread/condvar.h>
#include "registration.h"
#include "service.h"
#include "util.h"

DEFINE_string(service_type, "_daap._tcp",
              "");
DEFINE_int32(service_port, 0,
             "");
DEFINE_string(txt, "",
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
  map<string, string> txt_records;
  if (!btunnel::GetMap(FLAGS_txt, &txt_records)) {
    errx(1, "Invalid --txt flag");
  }
  btunnel::Service* service =
    btunnel::NewRegisteredService(FLAGS_service_type, service_port,
                                  txt_records);
  if (service == NULL) {
    errx(1, "Failed to create Service");
  }
  cout << "Service registered: " << service->name() << ", " << service->type()
       << ", " << service->domain() << endl;

  mutex_.Lock();
  condvar_.Wait();
  mutex_.Unlock();

  cout << "Unregistering service" << endl;
  delete service;
}
