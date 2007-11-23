// exported_service.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>

#include <map>
#include <err.h>
#include <stdlib.h>
#include <time.h>
#include <sysexits.h>
#include <ynet/select.h>
#include <ynet/tcp_server.h>
#include <iostream>

#include "exported_service.h"
#include "peer.h"
#include "peer_message.h"


using namespace std;

namespace btunnel {

ExportedService::ExportedService(ynet::Select* select,
                                 int port,
                                 int service_id,
                                 Peer* peer)
    : select_(select), port_(port), service_id_(service_id), peer_(peer) {
  srandom(time(NULL));
  tcp_server_ = new ynet::TCPServer(
      select, port, ythread::NewCallback(this, &ExportedService::Connect));
  tcp_server_->Start();
}

ExportedService::~ExportedService() {
  // Shutdown open clients
  while (socket_to_session_.size() > 0) {
    int sock = socket_to_session_.begin()->first;
    Close(sock);
  }
  tcp_server_->Stop();
  delete tcp_server_;
}

void ExportedService::Connect(ynet::Connection* conn) { 
  int session_id;
  assert(socket_to_session_.count(conn->sock) == 0);
  do {
    session_id = random() % kMaxServiceId;
  } while (session_to_socket_.count(session_id) != 0);
  socket_to_session_[conn->sock] = session_id;
  session_to_socket_[session_id] = conn->sock;
  select_->AddReadFd(conn->sock,
                     ythread::NewCallback(this, &ExportedService::Read));
}

void ExportedService::Read(int sock) {
  assert(socket_to_session_.count(sock) == 1);

  char buffer[kMaxBufLen];
  ssize_t nread = read(sock, buffer, kMaxBufLen);
  if (nread == -1) {
    err(EX_OSERR, "read() (%d)", sock);
  } else if (nread == 0) {
    warn("Connection closed on read");
    Close(sock);
    return;
  }

  ForwardRequest request;
  request.service_id = service_id_;
  request.session_id = socket_to_session_[sock];
  request.buffer = string(buffer, nread);
  if (!peer_->Forward(sock, &request)) {
    Close(sock);
  }
}

void ExportedService::Close(int sock) {
  assert(socket_to_session_.count(sock) == 1);
  select_->RemoveReadFd(sock);
  close(sock);
  int session_id = socket_to_session_[sock];
  assert(session_id > 0);
  socket_to_session_.erase(sock);
  session_to_socket_.erase(session_id);
}

}  // namespace btunnel
