// client.cpp

#include "client.h"

#include <mach/mach.h>
#include <servers/bootstrap.h>

namespace machrpc {

// An arbitrary buffer response size
#define MAX_MESSAGE_SIZE (1024)
uint8_t message_buffer[MAX_MESSAGE_SIZE];

// The swig wrapping code invokes these clear/check exception methods so that
// it can raise nicer python exceptions on mach errors.

kern_return_t mach_error_status = 0;

void SetException(kern_return_t error) {
  mach_error_status = error;
}

void ClearException() {
  SetException(KERN_SUCCESS);
}

char* CheckException() {
  if (mach_error_status == KERN_SUCCESS) {
    return NULL;
  }
  return mach_error_string(mach_error_status);
}


Client::Client() { }
Client::~Client() { }


class MachClient : public Client {
 public:
  explicit MachClient(mach_port_t server_port);

  virtual ~MachClient();

  virtual std::string SendMessage(
      mach_msg_id_t msgh_id,
      const std::string& request,
      int reply_size);

  void SendMessage(
      mach_msg_id_t msgh_id,
      const std::string& request);

 private:
  mach_port_t server_port_;
};

MachClient::MachClient(mach_port_t server_port) : server_port_(server_port) { }

MachClient::~MachClient() {
  // TODO(allen): Log if this fails?
  mach_port_deallocate(mach_task_self(), server_port_);
}

void MachClient::SendMessage(
    mach_msg_id_t msgh_id,
    const std::string& request) {
  SendMessage(msgh_id, request, 0);
}

std::string MachClient::SendMessage(
    mach_msg_id_t msgh_id,
    const std::string& request,
    int response_size) {
  typedef struct {
    mach_msg_header_t Head;
    NDR_record_t NDR;
  } Request;
  typedef struct {
    mach_msg_header_t Head;
    NDR_record_t NDR;
    kern_return_t RetCode;
    // The reply data actually does here before trailer.  We don't access the
    // trailer in this function so just use it for size calculations.
    char reply[0];
    mach_msg_trailer_t trailer;
  } Reply;
  size_t request_size = (request.size() + sizeof(Request));
  if (request_size > MAX_MESSAGE_SIZE) {
    SetException(MACH_SEND_TOO_LARGE);
    return "";
  }
  size_t reply_size = (mach_msg_size_t)(response_size + sizeof(Reply));
  if (reply_size > MAX_MESSAGE_SIZE) {
    SetException(MACH_RCV_TOO_LARGE);
    return "";
  }
  mach_msg_size_t message_size = (mach_msg_size_t)request_size;
  if (response_size > 0 && reply_size > request_size) {
    message_size = (mach_msg_size_t)reply_size;
  }
  bzero(message_buffer, message_size);
  Request *InP = (Request*)message_buffer;
  Reply *OutP = (Reply*)message_buffer;
  InP->NDR = NDR_record;
  InP->Head.msgh_bits = MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
  /* msgh_size passed as argument */
  InP->Head.msgh_remote_port = server_port_;
  InP->Head.msgh_local_port = mig_get_reply_port();
  InP->Head.msgh_id = msgh_id;
  memcpy(message_buffer + sizeof(Request), request.data(), request.size());
  int options = MACH_SEND_MSG|MACH_MSG_OPTION_NONE;
  if (reply_size > 0) {
    options |= MACH_RCV_MSG;
  }
  kern_return_t kr = mach_msg(&InP->Head,
      MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE,
      (mach_msg_size_t) request_size, (mach_msg_size_t) reply_size,
      InP->Head.msgh_local_port,
      MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
  if (kr != KERN_SUCCESS) {
    SetException(kr);
    return "";
  }
  if (OutP->RetCode != KERN_SUCCESS) {
    __NDR_convert__mig_reply_error_t((mig_reply_error_t *)OutP);
    SetException(OutP->RetCode);
    return "";
  }
  std::string s;
  s.assign((char*)(OutP->reply), response_size);
  return s;
}

Client* NewBootstrapClient() {
  return new MachClient(bootstrap_port);
}

Client* NewClient(const std::string& service_name) {
  mach_port_t server_port;
  kern_return_t kr =
      bootstrap_look_up(bootstrap_port, service_name.c_str(), &server_port);
  if (kr != BOOTSTRAP_SUCCESS) {
    SetException(kr);
    return NULL;
  }
  return new MachClient(server_port);
}

}  // namespace machrpc
