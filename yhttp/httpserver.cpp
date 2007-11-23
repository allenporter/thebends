#include "httpserver.h"
#include <vector>
#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sysexits.h>
#include <ythread/callback-inl.h>
#include <ythread/threadpool.h>
#include <ynet/select.h>
#include "reader.h"
#include "writer.h"
#include "util.h"

namespace yhttpserver {

class Request : public HTTPRequest {
 public:
  Request(HTTPServer* server, int sock)
    : server_(server), sock_(sock), input_(sock), output_(sock), ready_(false),
      valid_(true), wrote_header_(false), response_code_(200),
      content_type_("text/plain") { }
  virtual ~Request() { }

  virtual URL* Url() {
    return &url_;
  }

  virtual void SetResponseCode(int code) {
    response_code_ = code;
  }

  virtual void SetContentType(const std::string& type) {
    content_type_ = type;
  }

  virtual void WriteHeader() {
    char buf[BUFSIZ];
    snprintf(buf, BUFSIZ, "HTTP/1.0 %d OK\r\n", response_code_);
    output_.WriteString(buf, strlen(buf));
    snprintf(buf, BUFSIZ, "Content-Type: %s\r\n\r\n", content_type_.c_str());
    output_.WriteString(buf, strlen(buf));
  }

  virtual void WriteString(const std::string& output) {
    if (!wrote_header_) {
      WriteHeader();
      wrote_header_ = true;
    }
    output_.WriteString(output);
  }

  virtual void Reply() {
    output_.Flush();
    server_->Close(this);
  } 

  virtual void Read() {
    if (!input_.Read()) {
      fprintf(stderr, "Error during read\n");
      ready_ = true;
      valid_ = false;
      return;
    }
    const std::string& input = input_.Buffer();
    if (input.find("\n\n") < 0) {
      return;
    }
    ready_ = true;
    int pos = input.find('\n');
    std::vector<std::string> parts;
    Split(input.substr(0, pos), ' ', &parts);
    if (parts.size() != 3) {
      fprintf(stderr, "Wrong number of request parts: '%s'\n", input.c_str());
      valid_ = false;
      return; 
    }
    if (parts[0] != "GET" && parts[2].find("HTTP/") != 0) {
      fprintf(stderr, "Invalid header format");
      valid_ = false;
      return; 
    }
    std::string url = "http://localhost";
    url.append(parts[1]);
    url_.Parse(url);
  }

  bool Ready() const {
    return ready_;
  }

  bool Valid() const {
    return valid_;
  }

  int Sock() const {
    return sock_;
  }

 protected:
  HTTPServer* server_;
  int sock_;

  Reader input_;
  Writer output_;

  bool ready_;
  bool valid_;
  URL url_;
  std::string path_;

  bool wrote_header_;
  int response_code_;
  std::string content_type_;
};

HTTPServer::HTTPServer(ynet::Select* select, int port) : select_(select),
                                                         port_(port) {
  assert(select);
}

HTTPServer::~HTTPServer() { }

void HTTPServer::RegisterHandler(const std::string& handler,
                                 HandlerCallback* cb) {
  if (handlers_.find(handler) != handlers_.end()) {
    // override existing handler
    delete (handlers_[handler]);
    handlers_.erase(handler);
  }
  handlers_[handler] = cb;
}

void HTTPServer::Start() {
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    err(EX_OSERR, "socket()");
  }

  struct sockaddr_in name;
  memset(&name, 0, sizeof(struct sockaddr_in));
  name.sin_family = AF_INET;
  name.sin_port = htons(port_);
  name.sin_addr.s_addr = INADDR_ANY;

  int reuse = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                 (char*)&reuse, sizeof(int)) == -1) {
    err(EX_OSERR, "sotsockopt()");
  }
  int flags;
  if ((flags = fcntl(sock, F_GETFL, 0)) == -1) {
    err(EX_OSERR, "fcntl()");
  }
  if ((fcntl(sock, F_SETFL, O_NONBLOCK)) == -1) {
    err(EX_OSERR, "fcntl()");
  }
  if (bind(sock, (const struct sockaddr*)&name, sizeof(name)) == -1) {
    err(EX_OSERR, "bind");
  }
  if (listen(sock, SOMAXCONN) == -1) {
    err(EX_OSERR, "listen");
  }
  printf("Listening on port %d\n", port_);
  ynet::ReadyCallback* cb = ythread::NewCallback(this, &HTTPServer::Accept);
  select_->AddReadFd(sock, cb);
}

void HTTPServer::Accept(int sock) {
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);
  int client_sock;
  if ((client_sock = accept(sock, (struct sockaddr*)&client_addr,
                            &len)) == -1) {
    err(EX_OSERR, "accept");
  }
  printf("Accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));
  if (requests_.find(client_sock) != requests_.end()) {
    err(EX_SOFTWARE, "Accept on duplicate socket: %d\n", client_sock);
  }
  requests_[client_sock] = new Request(this, client_sock);
  ynet::ReadyCallback* cb = ythread::NewCallback(this, &HTTPServer::Read);
  select_->AddReadFd(client_sock, cb);
}

void HTTPServer::Read(int client_sock) {
  RequestMap::iterator request_it = requests_.find(client_sock);
  if (request_it == requests_.end()) {
    err(EX_SOFTWARE, "Read on unknown client_sock: %d", client_sock);
  }
  Request* request = request_it->second;
  request->Read();
  if (!request->Valid()) {
    Error(request);
    return;
  }
  if (!request->Ready()) {
    return;
  }
  HandlerMap::iterator handler_it = handlers_.find(request->Url()->Path());
  if (handler_it == handlers_.end()) {
    NotFound(request);
  } else {
    HandlerCallback* cb = handler_it->second;
    cb->Execute(request);
  }
}

void HTTPServer::Close(Request* request) {
  int sock = request->Sock();
  printf("Connection closed\n");
  select_->RemoveReadFd(sock);
  close(sock);
  requests_.erase(sock);
  delete request;
}

void HTTPServer::Error(HTTPRequest* request) {
  request->SetResponseCode(500);
  request->SetContentType("text/html");
  request->WriteString("<html><body>error</body></html>");
  request->Reply();
}

void HTTPServer::NotFound(HTTPRequest* request) {
  request->SetResponseCode(404);
  request->SetContentType("text/html");
  request->WriteString("<html><body>not found</body></html>");
  request->Reply();
}

void HTTPServer::Busy(HTTPRequest* request) {
  request->SetResponseCode(500);
  request->SetContentType("text/html");
  request->WriteString("<html><body>server busy</body></html>");
  request->Reply();
}


}  // namespace yhttpserver
