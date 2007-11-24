// peer_message.h
// Author: Allen Porter <allen@thebends.org>
//
// Protocol messages exchanged between bonjour tunnel peers.

#ifndef __BTUNNEL_PEER_MESSAGE_H__
#define __BTUNNEL_PEER_MESSAGE_H__

#include <string>
#include <sys/types.h>

namespace ynet {
class ReadBuffer;
class WriteBuffer;
}

namespace btunnel {

class Service;
class Peer;

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

int ReadRegister(ynet::ReadBuffer* buffer, RegisterRequest* request);
int WriteRegister(ynet::WriteBuffer* buffer, const RegisterRequest& request);

// Type UNREGISTER
struct UnregisterRequest {
  int32_t service_id;
};

int ReadUnregister(ynet::ReadBuffer* buffer, UnregisterRequest* request);
int WriteUnregister(ynet::WriteBuffer* buffer,
                    const UnregisterRequest& request);

// Type FORWARD
struct ForwardRequest {
  int32_t service_id;
  int32_t session_id;
  std::string buffer;
};

int ReadForward(ynet::ReadBuffer* buffer, ForwardRequest* request);
int WriteForward(ynet::WriteBuffer* buffer, const ForwardRequest& request);

// A MessageReader parses incoming requests and invokes the appropriate message
// on a peer.
class MessageReader {
 public:
  MessageReader(Peer* peer);
  ~MessageReader();

  // Returns the number of bytes read from the input buffer, or -1 if a parse
  // error occurred.
  int Read(int sock, ynet::ReadBuffer* buffer);

 private:
  int HandleRegister(int sock, ynet::ReadBuffer* buffer);
  int HandleUnregister(int sock, ynet::ReadBuffer* buffer);
  int HandleForward(int sock, ynet::ReadBuffer* buffer);

  Peer* peer_;
};

}  // namespace btunnel

#endif  // __BTUNNEL_PEER_MESSAGE_H__
