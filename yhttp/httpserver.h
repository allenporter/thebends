// httpserver.h
// Author: Allen Porter <allen@thebends.org>

#ifndef _YHTTPSERVER_H_
#define _YHTTPSERVER_H_

#include <string>
#include <map>
#include <ythread/callback.h>
#include "url.h"

namespace yhttpserver {

class Select;
class Request;
class HTTPRequest;

class HTTPServer {
 public:
  HTTPServer(Select* select, int port);
  virtual ~HTTPServer();

  typedef ythread::Callback1<HTTPRequest*> HandlerCallback;

  void RegisterHandler(const std::string& handler,
                       HandlerCallback* cb);

  void Start();

 protected:
  friend class Request;

  typedef std::map<std::string, HandlerCallback*> HandlerMap;
  typedef std::map<int, Request*> RequestMap;

  void Accept(int sock);
  void Read(int client_sock);
  void Close(Request* request);
  void Error(HTTPRequest* request);
  void NotFound(HTTPRequest* request);
  void Busy(HTTPRequest* request);

  Select* select_; 
  int port_;
  HandlerMap handlers_;
  RequestMap requests_;
};

class HTTPRequest {
 public:
  virtual ~HTTPRequest() { }

  virtual URL* Url() = 0;
  virtual void SetResponseCode(int code) = 0;
  virtual void SetContentType(const std::string& type) = 0;
  virtual void WriteString(const std::string& output) = 0;
  virtual void Reply() = 0;

 protected:
  // cannot be instantiated directly
  HTTPRequest() { }
};


}  // namespace yhttpserver

#endif  // _YHTTPSERVER_H_
