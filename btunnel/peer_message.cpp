// peer_message.cpp
// Author: Allen Porter <allen@thebends.org>

#include "peer_message.h"
#include <err.h>
#include <string>
#include "buffer.h"
#include "encoding.h"
#include "peer.h"

namespace btunnel {

MessageReader::MessageReader(Peer* peer) : peer_(peer) {

}

MessageReader::~MessageReader() {

}

int MessageReader::Read(int sock, Buffer* buffer) {
  int8_t type;
  if (!buffer->Read((char*)&type, 1)) {
    return 0;
  }
  int nbytes;
  if (type == REGISTER) {
    nbytes = HandleRegister(sock, buffer);
  } else if (type == FORWARD) {
    nbytes = HandleForward(sock, buffer);
  } else {
    warn("Invalid message type: %d", type);
    return -1;
  }
  if (nbytes == 0) {
    // Push back the "type" field
    buffer->Unadvance(1);  // type
    return 0;
  }
  return nbytes;
}

int MessageReader::HandleRegister(int sock, Buffer* buffer) {
  RegisterRequest request;
  int nbytes = ReadRegister(buffer, &request);
  if (nbytes > 0) {
    if (!peer_->Register(sock, &request)) {
      return -1;
    }
  }
  return nbytes;
}

int MessageReader::HandleForward(int sock, Buffer* inut_buffer) {
/*
  ForwardRequest request;
  int nbytes = ReadForward(buffer, &request);
  if (nbytes > 0) {
    if (!peer_->Forward(sock, &request)) {
      return -1;
    }
  }
  return nbytes;
*/
  return 0;
}

int ReadRegister(Buffer* buffer, RegisterRequest* request) {
  int nbytes = 0;
  if (!buffer->Read((char*)&request->service_id, sizeof(int32_t))) {
    return 0;
  }
  nbytes += sizeof(int32_t);
  request->service_id = ntohl(request->service_id);

  int ret = ReadString(buffer, kMaxNameLen, &request->name);
  if (ret <= 0) {
    buffer->Unadvance(nbytes);
    return ret;
  }
  nbytes += ret;

  ret = ReadString(buffer, kMaxTypeLen, &request->type);
  if (ret <= 0) {
    buffer->Unadvance(nbytes);
    return ret;
  }
  nbytes += ret;

  ret = ReadString(buffer, kMaxTextLen, &request->txt_records);
  if (ret <= 0) {
    buffer->Unadvance(nbytes);
    return ret;
  }
  nbytes += ret;
  return nbytes;
}

int WriteRegister(Buffer* buffer, const RegisterRequest& request) {
  int32_t service_id = htonl(request.service_id);
  int nbytes = 0;
  if (!buffer->Append((char*)&service_id, sizeof(int32_t))) {
    return -1;
  }
  nbytes += sizeof(int32_t);

  int ret = WriteString(buffer, kMaxNameLen, request.name);
  if (ret <= 0) {
    return -1;
  }
  nbytes += ret;

  ret = WriteString(buffer, kMaxTypeLen, request.type);
  if (ret <= 0) {
    return -1;
  }
  nbytes += ret;

  ret = WriteString(buffer, kMaxTextLen, request.txt_records);
  if (ret <= 0) {
    return -1;
  }
  nbytes += ret;
  return nbytes;
}




}  // namespace btunnel
