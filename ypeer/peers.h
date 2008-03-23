// peers.h
// Author: Allen Porter <allen@thebends.org>

#include <string>
#include <vector>

namespace ypeer {

struct Peer {
  std::string peer_id;
  std::string ip;
  int port;
};

class Peers {
 public:
  virtual ~Peers() { }

  virtual bool Connect() = 0;

  virtual bool GetPeers(std::vector<Peer>* peers) = 0;

 protected:
  Peers() { }
};

Peers* NewPeers(const std::string& tracker_url, const std::string& room);

}  // namespace ypeer
