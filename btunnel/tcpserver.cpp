// tcpserver.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>
#include "tcpserver.h"

#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sysexits.h>
#include <iostream>

using namespace std;

namespace btunnel {

TCPServer::TCPServer(yhttpserver::Select* select,
                     uint16_t port,
                     ConnectionCallback* callback)
  : select_(select), port_(port), sock_(-1), client_callback_(callback) {
}

TCPServer::~TCPServer() {
  Stop();
}

void TCPServer::Start() {
  if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    err(EX_OSERR, "socket()");
  }

  struct sockaddr_in name;
  memset(&name, 0, sizeof(struct sockaddr_in));
  name.sin_family = AF_INET;
  name.sin_port = htons(port_);
  name.sin_addr.s_addr = INADDR_ANY;

  int reuse = 1;
  if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR,
                 (char*)&reuse, sizeof(int)) == -1) {
    err(EX_OSERR, "sotsockopt()");
  }
  if ((fcntl(sock_, F_SETFL, O_NONBLOCK)) == -1) {
    err(EX_OSERR, "fcntl()");
  }

  // TODO: Set elsewhere?
  // 32k send buffer
  int buf = 32 * 1024;
  if (setsockopt(sock_, SOL_SOCKET, SO_SNDBUF, &buf, sizeof(buf)) != 0) {
    err(EX_OSERR, "setsockopt");
  }

  if (bind(sock_, (const struct sockaddr*)&name, sizeof(name)) == -1) {
    err(EX_OSERR, "bind");
  }
  if (listen(sock_, SOMAXCONN) == -1) {
    err(EX_OSERR, "listen");
  }
  cerr << "Listening on port " << port_ << endl;
  yhttpserver::Select::AcceptCallback* cb =
    ythread::NewCallback(this, &TCPServer::Accept);
  select_->AddFd(sock_, cb);
}

void TCPServer::Accept(int sock) {
  assert(sock_ == sock);
  Connection conn;
  bzero(&conn, sizeof(struct Connection));
  socklen_t len = sizeof(conn.addr);
  if ((conn.sock = accept(sock, (struct sockaddr*)&conn.addr, &len)) == -1) {
    err(EX_OSERR, "accept");
  }
  assert(len == sizeof(conn.addr));
  client_callback_->Execute(&conn);
}

void TCPServer::Stop() {
  if (sock_ != -1) {
    select_->RemoveFd(sock_);
    close(sock_);
  }
}

}  // namespace btunnel
