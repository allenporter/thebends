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
#include <map>
#include <google/gflags.h>
#include <ythread/mutex.h>
#include <ythread/condvar.h>
#include "registration.h"
#include "service.h"
#include "util.h"

DEFINE_string(name, "",
              "Name of the service.");
DEFINE_string(type, "_daap._tcp",
              "Type of service.");
DEFINE_int32(port, 0,
             "Local port on which the service accepts connections.");
DEFINE_string(txt, "",
              "Txt records for the service, comma separated name=value pairs.");

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

  if (FLAGS_name.empty()) {
    errx(1, "Required flag --name was missing");
  } else if (FLAGS_type.empty()) {
    errx(1, "Required flag --type was missing");
  } else if (FLAGS_port <= 0) {
    errx(1, "Required flag --port was missing");
  }

  signal(SIGINT, sig_handler);
  uint16_t port = FLAGS_port;
  map<string, string> txt_records;
  if (!btunnel::GetMap(FLAGS_txt, &txt_records)) {
    errx(1, "Invalid --txt flag");
  }

  btunnel::Service service(FLAGS_name, FLAGS_type, "", "", port, txt_records);
  btunnel::ServiceManager mgr;
  if (!mgr.Register(&service)) {
    errx(1, "Failed to register service");
  }
  cout << "Service registered: " << service.name() << " (" << service.type()
       << ", " << service.domain() << ")" << endl;


  mutex_.Lock();
  condvar_.Wait();
  mutex_.Unlock();

  cout << "Unregistering service" << endl;
  mgr.Unregister(&service);
}
