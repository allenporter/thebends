// tcp_client.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sysexits.h>
#include <iostream>

#include "tcp_client.h"
#include "select.h"
#include "util.h"

using namespace std;

namespace ynet {

TCPClient::TCPClient(Select* select,
                     struct in_addr remote_addr,
                     int16_t port,
                     ConnectionCallback* callback)
  : select_(select), addr_(remote_addr), port_(port), sock_(-1),
    client_callback_(callback), connected_(false) {
}

TCPClient::~TCPClient() {
  Stop();
}

void TCPClient::Connect() {
  if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    err(EX_OSERR, "socket()");
  }

  struct sockaddr_in name;
  memset(&name, 0, sizeof(struct sockaddr_in));
  name.sin_family = AF_INET;
  name.sin_port = htons(port_);
  name.sin_addr = addr_;

  cout << "Connecting to " << inet_ntoa(addr_) << ":" << port_ << endl;
  ynet::SetNonBlocking(sock_);
  int ret = connect(sock_, (const struct sockaddr*)&name, sizeof(name));
  // We expect EINPROGRESS since this is non-blocking
  if (ret != -1) {
    errx(1, "Unexpected connect() return value: %d", ret);
  }
  if (errno != EINPROGRESS) {
    err(1, "connect");
  }
  ReadyCallback* cb = ythread::NewCallback(this, &TCPClient::Ready);
  select_->AddWriteFd(sock_, cb);
}

void TCPClient::Ready(int sock) {
  assert(sock_ == sock);
  select_->RemoveWriteFd(sock_);
  Connection conn;
  bzero(&conn, sizeof(struct Connection));
  conn.sock = sock_;
  socklen_t len = sizeof(conn.addr);
  if (getpeername(sock_, (struct sockaddr*)&conn.addr, &len) == 0) {
    cout << "Connected to " << inet_ntoa(addr_) << ":" << port_ << endl;
    client_callback_->Execute(&conn);
  } else {
    assert(len == sizeof(conn.addr));
    cout << "Failed connect to " << inet_ntoa(addr_) << ":" << port_ << endl;
    client_callback_->Execute(NULL);
  }
}

void TCPClient::Stop() {
  if (sock_ != -1) {
    close(sock_);
    sock_ = -1;
  }
}

}  // namespace ynet
