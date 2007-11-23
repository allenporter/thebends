#include <ythread/threadpool.h>
#include <ythread/callback-inl.h>
#include "httpserver.h"
#include "select.h"

class SimpleServer {
 public:
  SimpleServer() { }
  virtual ~SimpleServer() { }

  void Start() {
    printf("Starting server\n");
    ynet::Select select;
    yhttpserver::HTTPServer server(&select, 8080);

    server.RegisterHandler(
      "", ythread::NewCallback(this, &SimpleServer::HandleRequest));
    server.RegisterHandler(
      "foo", ythread::NewCallback(this, &SimpleServer::Foo));
    server.Start();
    select.Start();
  }

  void HandleRequest(yhttpserver::HTTPRequest* request) {
    request->SetContentType("text/html");
    request->WriteString("<html><body>hello world!</body></html>");
    request->Reply();
  }

  void Foo(yhttpserver::HTTPRequest* request) {
    request->SetContentType("text/plain");
    request->WriteString("foo");
    request->Reply();
  }
};

int main(int argc, char* argv[]) {
  SimpleServer server;
  server.Start();
}
