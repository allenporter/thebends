// multicast_capture
// Author: Allen Porter <allen@thebends.org>
//
// A simple tool for capturing multicast packets.  This is mostly used for
// debugging the multicast socket library.

#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sysexits.h>
#include <iostream>
#include "google/gflags.h"
#include "listener.h"
#include "multicast.h"
#include "util.h"

using namespace std;

DEFINE_string(address, "238.0.0.251",
              "The multicast group to sniff (Bonjour by default)");
DEFINE_int32(port, 5353,
             "Port to listen on");
DEFINE_int32(max_length, 32,
             "The maximum length of packets to display");

static ythread::Thread* thread = NULL;

void signal_handler(int signal) {
  assert(signal == SIGINT);
  if (thread) {
    cerr << "SIGINT; Shutting down" << endl;
    thread->Stop();
  }
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  signal(SIGINT, &signal_handler);
  if (FLAGS_port <= 0) {
    errx(EX_USAGE, "Required flag --port must be > 0");
  }
  if (FLAGS_address.empty()) {
    FLAGS_address = "0.0.0.0";  // INADDR_ANY
  }
  in_addr_t addr = inet_network(FLAGS_address.c_str());
  btunnel::PacketDumper dumper(FLAGS_max_length);
  btunnel::Socket* socket = btunnel::NewMulticastSocket(addr, FLAGS_port);
  thread = 
    btunnel::NewSocketListenerThread(socket->sock(), dumper.callback());

  thread->Start();
  cerr << "Listening..." << endl;
  thread->Join();  // Block forever (or until ^C)
  delete thread;
  thread = NULL;
  delete socket;
  socket = NULL;
}
