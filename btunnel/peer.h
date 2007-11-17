// peer.h
// Author: Allen Porter <allen@thebends.org>

#include <map>

namespace yhttpserver {
  class Select;
}

namespace btunnel {

class Buffer;

class Peer {
 public:
  Peer(yhttpserver::Select* select);
  ~Peer();

  void AddPeer(int sock);
  void RemovePeer(int sock);

 protected:
  Buffer* GetBuffer(int sock);
  void Read(int sock);
  void Close(int sock);

  bool HandleRegister(int sock);
  bool HandleForward(int sock);

  typedef std::map<int, Buffer*> PeerMap;
  yhttpserver::Select* select_;
  PeerMap peers_;
};

}  // namespace btunnel
