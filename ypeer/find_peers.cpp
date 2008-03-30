// peers.cpp
// Author: Allen Porter <allen@thebends.org>

#include <iostream>
#include "peers.h"
#include "sha1.h"

using namespace std;

static void Hash(uint32_t *hash, string* result) {
  for (int i = 0; i < 5; i++) {
    uint32_t key = hash[i];
    for (int j = 3; j >= 0; j--) {
      unsigned char c = (unsigned char)((key >> (j * 8)) & 0xff);
      result->append(1, c);
    }
  }
}
 
int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " <url> <room>" << endl;
    return 1;
  }
  const string& room = argv[2];
  uint32_t hash[5];
  SHA1 sha1;
  sha1.Input(room.c_str(), room.size());
  if (!sha1.Result(hash)) {
    cerr << "Unable to compute hash" << endl;
    return 1;
  }
  string info_hash;
  Hash(hash, &info_hash);
 
  ypeer::Peers* peers = ypeer::NewPeers(argv[1], info_hash, getpid());
  if (peers == NULL) {
    cerr << "Invalid url" << endl;
    return 1;
  }
  while (true) {
    vector<ypeer::Peer> active;
    if (!peers->GetPeers(&active)) {
      cout << "Failed to get peers" << endl;
      return 0;
    }
    cout << endl;
    cout << "Current peer list:" << endl;
    for (vector<ypeer::Peer>::const_iterator iter = active.begin();
         iter != active.end(); ++iter) {
      const ypeer::Peer& peer = *iter;
      cout << "Peer: " << peer.ip << ":" << peer.port << endl;
    }
    sleep(5);
  }
  return 0;
}
