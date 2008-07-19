// peers.h
// Author: Allen Porter <allen@thebends.org>
//
// Utility methods for finding network peers.

#ifndef __YPEER_PEER_H__
#define __YPEER_PEER_H__

#include <string>
#include <vector>
#include <ythread/callback-inl.h>

namespace ypeer {

struct Peer {
  std::string peer_id;
  std::string ip;
  int port;
};

class Peers {
 public:
  virtual ~Peers() { }

  // Returns true if an error occurred on the last attempt to find peers.
  virtual bool Error() = 0;
  virtual std::string ErrorString() = 0;

  // Returns true if the last connection was successful.
  virtual bool Connected() = 0;

  // Returns a list of peers.  May be empty if we haven't yet made a
  // connection with the server.  Returns false if any error occurred while
  // trying to get the list of peers (the tracker went down, or sent back
  // an unparseable message, etc).
  virtual bool GetPeers(std::vector<Peer>* peers) = 0;

  // Set a callback to be invoked when the list of peers changes.  The
  // callback function must not block, as it is invoked from an interal
  // network thread.
  virtual void SetCallback(ythread::Callback* callback) = 0;

 protected:
  Peers() { }
};

Peers* NewPeers(const std::string& tracker_url,
                const std::string& info_hash,
                int16_t local_port);

}  // namespace ypeer

#endif  // __YPEER_PEER_H__
