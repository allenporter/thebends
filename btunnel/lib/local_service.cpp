// local_service.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>

#include <arpa/inet.h>
#include <err.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ynet/select.h>
#include <ynet/tcp_server.h>
#include <ynet/buffered_writer.h>
#include <ynet/util.h>
#include <map>
#include <iostream>

#include "local_service.h"
#include "peer.h"
#include "peer_message.h"

using namespace std;

namespace btunnel {

static const int kBufferSize = 5 * 1024 * 1024;  // 5MB


LocalService::LocalService(ynet::Select* select,
                           int service_id,
                           struct in_addr addr,
                           uint16_t port,
                           Peer* peer)
    : select_(select), service_id_(service_id), port_(port), peer_(peer) {
  bzero(&addr_, sizeof(struct sockaddr_in));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = addr.s_addr;
  addr_.sin_port = htons(port);
}

LocalService::~LocalService() {
  while (socket_to_session_.size() > 0) {
    int sock = socket_to_session_.begin()->first;
    Close(sock);
  }
  assert(socket_to_session_.size() == 0);
  assert(session_to_socket_.size() == 0);
}

void LocalService::Read(int sock) {
  assert(socket_to_session_.count(sock) == 1);
  char buffer[kMaxBufLen];
  ssize_t nread = read(sock, buffer, kMaxBufLen);
  if (nread == -1) {
    err(EX_OSERR, "read() (%d)", sock);
  } else if (nread == 0) {
    warnx("LocalService Connection closed on read");
    Close(sock);
    return;
  }
  cout << "Local service read (" << nread << ")" << endl;

  ForwardRequest request;
  request.service_id = service_id_;
  request.session_id = socket_to_session_[sock];
  request.buffer = string(buffer, nread);
  if (!peer_->Forward(sock, &request)) {
    Close(sock);
  }
}

bool LocalService::Forward(const ForwardRequest* request) {
  // TODO: Non-blocking connect
  assert(request->service_id == service_id_);
  if (session_to_socket_.count(request->session_id) == 0) {
    // Establish a new connection
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
      err(1, "socket");
    }
    int ret = connect(sock, (struct sockaddr*)&addr_,
                      sizeof(struct sockaddr_in));
    if (ret != 0) {
      warn("connect");
      return false;
    }
    // TODO: Use tcp_client instead
    cout << "Local service connection created" << endl;
    assert(socket_writers_.count(sock) == 0);
    socket_writers_[sock] = new ynet::BufferedWriter(select_, sock,
                                                     kBufferSize);
    socket_to_session_[sock] = request->session_id;
    session_to_socket_[request->session_id] = sock;
    select_->AddReadFd(sock,
                       ythread::NewCallback(this, &LocalService::Read));
  }
  cout << "Local service write (" << request->buffer.size() << ")"
       << endl;
  int sock = session_to_socket_[request->session_id];
  ynet::BufferedWriter* writer = socket_writers_[sock];
  if (!writer->Write(request->buffer.data(), request->buffer.size())) {
    Close(sock);
    return false;
  }
  return true;
}

void LocalService::Close(int sock) {
  assert(socket_to_session_.count(sock) == 1);
  select_->RemoveReadFd(sock);
  close(sock);
  int session_id = socket_to_session_[sock];
  assert(session_id > 0);
  socket_to_session_.erase(sock);
  session_to_socket_.erase(session_id);
  delete socket_writers_[sock];
  socket_writers_.erase(sock);
}

}  // namespace btunnel
