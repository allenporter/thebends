// tunnel.cpp
// Author: Allen Porter <allen@thebends.org>

#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>

#include <iostream>
#include <map>

#include <ythread/callback-inl.h>

#include "tunnel.h"
#include "select.h"
#include "tcp_server.h"

using namespace std;

namespace ynet {

typedef map<int, int> SockMap;

class TcpTunnel : public Tunnel {
 public:
  TcpTunnel(Select* select,
            struct in_addr remote_addr,
            uint16_t remote_port,
            uint16_t local_port) : select_(select) {
    ConnectionCallback* accept_callback =
      ythread::NewCallback(this, &TcpTunnel::ClientConnected);
    server_ = new TCPServer(select, local_port, accept_callback);

    // Setup remote address
    bzero(&remote_, sizeof(struct sockaddr_in));
    remote_.sin_family = AF_INET;
    memcpy(&remote_.sin_addr, &remote_addr, sizeof(struct in_addr));
    remote_.sin_addr.s_addr = remote_addr.s_addr;
    remote_.sin_port = htons(remote_port);
  }

  virtual ~TcpTunnel() {
    Stop();
  }

  virtual void Start() {
    server_->Start();
  }

  void ClientConnected(struct Connection* client) {
    cout << "Accepted connection from " << inet_ntoa(client->addr.sin_addr)
         << endl;
    assert(pairs_.count(client->sock) == 0);

    // We've received a client connection; Establish a new connection to the
    // remote address
    int remote_sock;
    if ((remote_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      close(client->sock);
      err(EX_OSERR, "socket()");
    }
    // TODO: Open remote connection asyncronously
    cout << "Openining new remote connection" << endl;
    if (connect(remote_sock, (const struct sockaddr*)&remote_,
                sizeof(struct sockaddr_in)) != 0) {
      err(EX_OSERR, "connect()");
    }
    cout << "Connection established. (remote=" << remote_sock
         << ",client=" << client->sock << ")" << endl;
    // Setup socket pairs pairings
    SetForward(client->sock);
    SetForward(remote_sock);
    pairs_[remote_sock] = client->sock;
    pairs_[client->sock] = remote_sock;
  }

  void SetForward(int sock) {
    ReadyCallback* cb = ythread::NewCallback(this, &TcpTunnel::Read);
    select_->AddReadFd(sock, cb);
  }

  void Read(int sock) {
    SockMap::const_iterator iter = pairs_.find(sock);
    assert(iter != pairs_.end());
    int sock_pair = iter->second;

    char buf[BUFSIZ];
    ssize_t nread = read(sock, buf, BUFSIZ);
    if (nread == -1) {
      err(EX_OSERR, "read() (%d)", sock);
      return;
    } else if (nread == 0) {
      cerr << "Connection closed on read" << endl;
      Close(sock);
      Close(sock_pair);
      return;
    }
    usleep(10);
tryagain:
    ssize_t nwrote = write(sock_pair, buf, nread);
    if (nwrote == -1) {
      if (errno == EAGAIN) {
        // TODO: Total hack, use a write buffer instead
        usleep(100);
        goto tryagain;
      }
      err(EX_OSERR, "write() (%d)", sock_pair);
      return;
    } else if (nwrote == 0) {
      cerr << "Connection closed on write" << endl;
      Close(sock);
      Close(sock_pair);
      return;
    }
    assert(nwrote == nread);
  }

  void Close(int sock) {
    select_->RemoveReadFd(sock);
    close(sock);
    pairs_.erase(sock);
  }

  virtual void Stop() {
    server_->Stop();
  }

  struct sockaddr_in remote_;
  Select* select_;
  TCPServer* server_;
  SockMap pairs_;
};

Tunnel* NewTcpTunnel(Select* select,
                     struct in_addr addr,
                     uint16_t remote_port,
                     uint16_t local_port) {
  return new TcpTunnel(select, addr, remote_port, local_port);
}

}  // namespace ynet
