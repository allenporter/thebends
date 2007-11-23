// tcp_server.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>

#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sysexits.h>
#include <iostream>

#include "tcp_server.h"
#include "select.h"

using namespace std;

namespace ynet {

TCPServer::TCPServer(Select* select,
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

  if (bind(sock_, (const struct sockaddr*)&name, sizeof(name)) == -1) {
    err(EX_OSERR, "bind");
  }
  if (listen(sock_, SOMAXCONN) == -1) {
    err(EX_OSERR, "listen");
  }
  cerr << "Listening on port " << port_ << endl;
  ReadyCallback* cb = ythread::NewCallback(this, &TCPServer::Accept);
  select_->AddReadFd(sock_, cb);
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
    select_->RemoveReadFd(sock_);
    close(sock_);
    sock_ = -1;
  }
}

}  // namespace ynet
