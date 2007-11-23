// peer_message.h
// Author: Allen Porter <allen@thebends.org>
//
// Protocol messages exchanged between bonjour tunnel peers.

#ifndef __BTUNNEL_PEER_MESSAGE_H__
#define __BTUNNEL_PEER_MESSAGE_H__

#include <string>
#include <sys/types.h>

namespace btunnel {

class Service;
class Peer;
class Buffer;

// Type identifiers are one byte
enum Type {
  REGISTER = 0x10,
  UNREGISTER = 0x11,
  FORWARD = 0x20
};

static const int kMaxServiceId = 0x7fffffff;
static const int kMaxNameLen = 1024;
static const int kMaxTypeLen = 1024;
static const int kMaxTextLen = 8 * 1024;
static const int kMaxBufLen = 1300;

// Type REGISTER
struct RegisterRequest {
  int32_t service_id;
  std::string name;
  std::string type;
  std::string txt_records;
};

int ReadRegister(Buffer* buffer, RegisterRequest* request);
int WriteRegister(Buffer* buffer, const RegisterRequest& request);

// Type UNREGISTER
struct UnregisterRequest {
  int32_t service_id;
};

int ReadUnregister(Buffer* buffer, UnregisterRequest* request);
int WriteUnregister(Buffer* buffer, const UnregisterRequest& request);

// Type FORWARD
struct ForwardRequest {
  int32_t service_id;
  int32_t session_id;
  std::string buffer;
};

int ReadForward(Buffer* buffer, ForwardRequest* request);
int WriteForward(Buffer* buffer, const ForwardRequest& request);

// A MessageReader parses incoming requests and invokes the appropriate message
// on a peer.
class MessageReader {
 public:
  MessageReader(Peer* peer);
  ~MessageReader();

  // Returns the number of bytes read from the input buffer, or -1 if a parse
  // error occurred.
  int Read(int sock, Buffer* buffer);

 private:
  int HandleRegister(int sock, Buffer* buffer);
  int HandleUnregister(int sock, Buffer* buffer);
  int HandleForward(int sock, Buffer* buffer);

  Peer* peer_;
};

}  // namespace btunnel

#endif  // __BTUNNEL_PEER_MESSAGE_H__
