// ydb_server.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>

#include <err.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ythread/threadpool.h>
#include "ydb.h"

void handle_client(void * args) {
  int sock = (int)args;

  u_char buf[BUFSIZ];
  ssize_t msg_size;
  if ((msg_size = recv(sock, buf, BUFSIZ, MSG_WAITALL)) == -1) {
    err(1, "recvmsg(): %s", strerror(errno));
  }

  YDEBUG("New client!");
  sleep(10);
  YDEBUG("Client done!");

  close(sock);
}

void deny_client(int sock) {
  YDEBUG("Denied client");
  close(sock);
}

void start_server(int port) {
  int sock;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    err(1, "socket(): %s", strerror(errno));
  }

  struct sockaddr_in name;
  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = INADDR_ANY;

  int reuse = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                 (char*)&reuse, sizeof(int)) == -1) {
    err(1, "setsockopt(): %s", strerror(errno));
  }

  if (bind(sock, (const struct sockaddr*)&name, sizeof(name)) == -1) {
    err(1, "bind(): %s", strerror(errno));
  }

  if (listen(sock, 10) == -1) {
    err(1, "listen(): %s", strerror(errno));
  }

  ythread::ThreadPool pool(4);  // 4 workers
  YDEBUG("Listening on " << port);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_sock;
    if ((client_sock = accept(sock, (struct sockaddr*)&client_addr,
                              &len)) == -1) {
      err(1, "accept(): %s", strerror(errno));
    }

    YDEBUG("Client connected");
    if (!pool.TryAdd(ythread::NewCallback(&handle_client,
                                          (void*)client_sock))) {
      // Deny requests if the thread pool is full.
      deny_client(client_sock);
    }
  }
}

void usage(const char* appname) {
  cerr << "usage: " << appname << " [-p port] " << endl;
  exit(1);
}

int main(int argc, char* argv[]) {
  const char* appname = argv[0];
  int server_port = 0;

  char ch;
  while ((ch = getopt(argc, argv, "p:")) != -1) {
    switch (ch) {
      case 'p':
        server_port = (int)strtol(optarg, (char **)NULL, 10);
        if (errno != 0) {
          server_port = 0;
        }
        break;
      case '?':
      default:
        usage(appname);  // exits
    }
  }
  argc -= optind;
  argv += optind;

  if (server_port <= 0) {
    usage(appname);  // exists
  }

  start_server(server_port);
}
