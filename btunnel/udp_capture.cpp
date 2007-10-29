// udp_capture
// Author: Allen Porter <allen@thebends.org>
//
// A simple tool for capturing udp packets.  This is mostly used for debugging
// the udp socket library.

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
#include "udp.h"
#include "util.h"

using namespace std;

DEFINE_int32(port, 9999,
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
  btunnel::PacketDumper dumper(FLAGS_max_length);
  btunnel::Socket* socket = btunnel::NewUdpSocket(FLAGS_port);
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
