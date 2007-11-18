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

struct MessageHeader {
  int8_t type;
};

struct Response {
  int32_t status;
};

static const int kMaxNameLen = 1024;
static const int kMaxTypeLen = 1024;
static const int kMaxTextLen = 8 * 1024;

// Type REGISTER
struct RegisterRequest {
  int32_t service_id;
  std::string name;
  std::string type;
  std::string txt_records;
};

// Type UNREGISTER
struct UnregisterRequest {
  int32_t id;
};

// Type FORWARD
struct ForwardRequest {
  int32_t id;
  int16_t msg_size;
  char msg[0];
};

// A MessageReader parses incoming requests and invokes the appropriate message
// on a peer.
class MessageReader {
 public:
  MessageReader(Peer* peer);
  ~MessageReader();

  // Returns the number of bytes read from the input buffer, or -1 if a parse
  // error occurred.
  int Read(int sock, Buffer* input_buffer);

 private:
  int HandleRegister(int sock, Buffer* input_buffer);
  int HandleForward(int sock, Buffer* input_buffer);

  Peer* peer_;
};

int ReadRegister(Buffer* buffer, RegisterRequest* request);

int WriteRegister(Buffer* buffer, const RegisterRequest& request);


}  // namespace btunnel

#endif  // __BTUNNEL_PEER_MESSAGE_H__
