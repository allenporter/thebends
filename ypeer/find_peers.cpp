// peers.cpp
// Author: Allen Porter <allen@thebends.org>

#include <iostream>
#include "peers.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " <url> <room>" << endl;
    return 1;
  }
  ypeer::Peers* peers = ypeer::NewPeers(argv[1], argv[2]);
  if (peers == NULL) {
    cerr << "Invalid url" << endl;
    return 1;
  }
  if (!peers->Connect()) {
    cerr << "Connection failed" << endl;
    return 1;
  }
  return 0;
}
