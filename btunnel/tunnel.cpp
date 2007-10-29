// tunnel.cpp
// Author: Allen Porter <allen@thebends.org>
#include "tunnel.h"

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include "listener.h"
#include "packer.h"
#include "writer.h"
#include "udp.h"

using namespace std;

namespace btunnel {

class UdpTunnel : public Tunnel {
 public:
  UdpTunnel(struct in_addr remote_addr,
            uint16_t remote_port,
            uint16_t local_port,
            Packer* packer,
            ReceivedCallback* client_callback)
      : socket_(NewUdpSocket(local_port)),
        packer_(packer),
        client_callback_(client_callback) {
    // Setup remote address
    bzero(&remote_, sizeof(struct sockaddr_in));
    remote_.sin_family = AF_INET;
    remote_.sin_addr.s_addr = htonl(remote_addr.s_addr);
    remote_.sin_port = htons(remote_port);

    received_callback_ = 
      ythread::NewCallback(this, &UdpTunnel::Received);
    thread_ = NewSocketListenerThread(socket_->sock(), received_callback_);
    writer_ = NewSocketWriter(socket_->sock());
  }

  virtual ~UdpTunnel() {
    thread_->Stop();
    thread_->Join();
    delete thread_;
    delete client_callback_;
    delete received_callback_;
    delete writer_;
    delete socket_;
  }

  virtual void Write(const unsigned char* data,
                     size_t length) {
// TODO: wrap and send
//    writer_->Write(remote_, packed_data, packed_length);
  }

 private:
  void Received(struct packet_info* wrapped_info) {
    const struct sockaddr_in& sender = wrapped_info->sender;
    if (remote_.sin_addr.s_addr != sender.sin_addr.s_addr &&
        remote_.sin_port != sender.sin_port) {
      cerr << "Ignoring packet from " << inet_ntoa(sender.sin_addr)
           << ":" << ntohs(sender.sin_port) << endl;
      return;
    }

    struct packet_info info;
    info.sender = wrapped_info->sender;
    packer_->Pack(wrapped_info->data, wrapped_info->length,
                  info.data, &info.length);
    client_callback_->Execute(&info);
  }

  Socket* socket_;
  Packer* packer_;
  struct sockaddr_in remote_;
  ReceivedCallback* client_callback_;
  ReceivedCallback* received_callback_;
  ythread::Thread* thread_;
  Writer* writer_;
};

Tunnel* NewUdpTunnel(struct in_addr addr,
                     uint16_t remote_port,
                     uint16_t local_port,
                     Packer* packer,
                     ReceivedCallback* callback) {
  return new UdpTunnel(addr, remote_port, local_port, packer, callback);
}

}  // namespace btunnel
