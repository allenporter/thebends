// peer.h
// Author: Allen Porter <allen@thebends.org>

#include <map>

namespace yhttpserver {
  class Select;
}

namespace btunnel {

class Buffer;
class RegisterRequest;
class UnregisterRequest;
class ForwardRequest;

class Peer {
 public:
  virtual ~Peer() { }

  virtual bool Register(int sock, const RegisterRequest* request) = 0;

  virtual bool Unregister(int sock, const UnregisterRequest* request) = 0;

  virtual bool Forward(int sock, const ForwardRequest* request) = 0;

 protected:
  // Cannot be instantiated directly
  Peer() { }
};

class PeerConnection {
 public:
  PeerConnection(yhttpserver::Select* select);
  ~PeerConnection();

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
