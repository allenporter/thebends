// peer_message.h
// Author: Allen Porter <allen@thebends.org>
//
// Protocol messages exchanged between bonjour tunnel peers.

#ifndef __BTUNNEL_PEER_MESSAGE_H__
#define __BTUNNEL_PEER_MESSAGE_H__

namespace btunnel {

class Service;

// Type identifiers are one byte
enum Type {
  REGISTER = 0x10,
  UNREGISTER = 0x11,

  FORWARD = 0x20
};

struct MessageHeader {
  uint8_t type;
};

// Type REGISTER
struct RegisterBody {
  uint8_t type;
  int32_t id;
/*
  char name[NAMELEN];
  char type[TYPELEN];
  const std::string& txt;
*/
};

// Type UNREGISTER
struct UnregisterBody {
  uint8_t type;
  uint32_t id;
// TODO
//  Service* service; 
};

// Type FORWARD
struct ForwardBody {
  uint8_t type;
  uint32_t id;
  uint8_t msg[BUFSIZ];
  uint16_t msg_size;
};

// This struct is only used to calculate the largest message size.
struct MessageUnion {
  union Body {
    struct RegisterBody;
    struct UnregisterBody;
    struct ForwardBody;
  };
};

}  // namespace btunnel

#endif  // __BTUNNEL_PEER_MESSAGE_H__
