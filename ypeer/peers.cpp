// peers.cpp
// Author: Allen Porter <allen@thebends.org>

#include "peers.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <ctype.h>
#include <err.h>
#include <sysexits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <yhttp/httpserver.h>
#include <ynet/buffer.h>
#include <ythread/callback-inl.h>
#include <ythread/condvar.h>
#include <ythread/mutex.h>
#include <ythread/thread.h>
#include "response.h"
#include "http_response.h"

using namespace std;

namespace ypeer {

class CBThread : public ythread::Thread {
 public:
  CBThread(ythread::Callback* cb) : cb_(cb) { }

 protected:
  virtual void Run() {
    cb_->Execute();
  }

 private:
  ythread::Callback* cb_;
};

class PeersImpl : public Peers {
 public:
  PeersImpl(const string& host,
            int16_t port,
            const string& path,
            const string& info_hash,
            int16_t local_port)
      : host_(host),
        port_(port),
        path_(path),
        info_hash_(info_hash),
        local_port_(local_port),
        thread_(ythread::NewCallback(this, &PeersImpl::StartInThread)),
        shutdown_(false),
        error_(false) {
    thread_.Start();
  }

  ~PeersImpl() {
    mu_.Lock();
    shutdown_ = true;
    mu_.Unlock();
    thread_.Join();
  }

  bool GetPeers(vector<Peer>* peers) {
    mu_.Lock();
    if (error_) {
      mu_.Unlock();
      return false;
    }
    *peers = peers_;
    mu_.Unlock();
    return true;
  }

  void SetCallback(ythread::Callback* callback) {
    // TODO(allen): implement this when its needed
  }

 private:
  void SetError() {
    mu_.Lock();
    error_ = true;
    mu_.Unlock();
  }

  string GetPortString() {
    char buf[BUFSIZ];
    sprintf(buf, "%d", local_port_);
    return string(buf);
  }

  int ConnectAndSendMessage(in_addr addr, const string& message) {
    struct sockaddr_in name;
    memset(&name, 0, sizeof(struct sockaddr_in));
    name.sin_family = AF_INET;
    name.sin_port = htons(port_);
    name.sin_addr = addr;

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      err(EX_OSERR, "socket()");
    }

    int ret = connect(sock, (const struct sockaddr*)&name, sizeof(name));
    if (ret == -1) {
      cerr << "Failed to connect to host" << endl;
      return -1;
    }

    size_t s = write(sock, message.data(), message.size());
    if (s != message.size()) {
      cerr << "write size mismatch " << s << " != " << message.size() << endl;
      return -1;
    }
    return sock;
  }

  bool Read(int sock, ynet::Buffer* buffer) {
    char buf[1024];
    int nbytes;
    do {
      nbytes = read(sock, buf, 1024);
      if (nbytes == -1) {
        perror("read");
        return false;
      } else if (nbytes == 0) {
        return true;
      }
      buffer->Write(buf, nbytes);
    } while (true);
  }

  bool GetResponse(int sock, string* response) {
    ynet::Buffer buf;
    if (!Read(sock, &buf)) {
      cout << "Error reading" << endl;
      return false;
    }
    // This could be more efficient, but i'd rather just re-use ynet code
    // for now
    char tmp[1024];
    while (buf.Size() > 0) {
      int nread = (buf.Size() > 1024) ? 1024 : buf.Size();
      buf.Read(tmp, nread);
      response->append(tmp, nread);
    }
    return true;
  }
  void BuildRequest(const map<string, string>& params, string* message) {
    message->append("GET /");
    message->append(path_);
    message->append("?");
    AppendParams(params, message);
    message->append(" HTTP/1.1\n");
    message->append("Host: ");
    message->append(host_);
    message->append("\n");
    message->append("Connection: close\n");
    message->append("User-Agent: libypeer\n");
    message->append("\n");
  }


  void StartInThread() {
    hostent* hp = gethostbyname(host_.c_str());
    if (hp == NULL) {
      cerr << "Unable to resolve hostname " << host_ << endl;
      return;
    }
    in_addr addr;
    memcpy(&addr, hp->h_addr, sizeof(struct in_addr));

    map<string, string> params;
    params["info_hash"] = info_hash_;
    params["peer_id"] = GeneratePeerId();
    params["port"] = GetPortString();
    params["uploaded"] = "0";
    params["downloaded"] = "100";
    params["left"] = "100";
    params["compact"] = "1";
    params["numwant"] = "50";
    params["event"] = "start";

    mu_.Lock();
    while (!shutdown_) {
      mu_.Unlock();

      string message;
      BuildRequest(params, &message);

      int sock;
      if ((sock = ConnectAndSendMessage(addr, message)) < 0) {
        SetError();
        return;
      }
      
      string http_response;
      if (!GetResponse(sock, &http_response)) {
        close(sock);
        cerr << "Failed to get HTTP response" << endl;
        SetError();
        return;
      }
      close(sock);

      string body;
      if (!ParseHTTPResponse(http_response, &body)) {
        cerr << "Parsing HTTP request failed" << endl;
        SetError();
        return;
      }

      ResponseMessage result;
      if (!ParseResponseMessage(body, &result)) {
        cerr << "Failed to parse tracker response" << endl;
        SetError();
        return;
      }
      if (!result.failure_reason.empty()) {
        cerr << "Failure from tracker: " << result.failure_reason << endl;
        SetError();
        return;
      }

      if (!result.warning_message.empty()) {
        cout << "Warning: " << result.warning_message << endl;
      }

      // TODO(allen): Keep a list of peers across tracker requests (in a set)
      cout << "Updating internal peers list with " << result.peers.size()
           << " peers.  Will check again in " << result.interval << " seconds."
           << endl;
      mu_.Lock();
      peers_ = result.peers;
      mu_.Unlock();

      // Update parameters for next request
      params["trackerid"] = result.tracker_id;
      params.erase("event");
      sleep(result.interval);
    }
  }

  string GeneratePeerId() {
    srandom(time(NULL) + getpid());
    string id;
    id.append("-YP-");
    while (id.size() < 20) {
      id.append(1, '0' + (random() % 10));
    }
    return id;
  }

  void AppendParams(const map<string, string>& params,
                    string* message) {
    int i = 0;
    for (map<string, string>::const_iterator it = params.begin();
         it != params.end(); ++it) {
      if (i != 0) {
        message->append("&");
      }
      Escape(it->first, message);
      message->append("=");
      Escape(it->second, message);
      i++;
    }
  }

  void Escape(const string& value, string* message) {
    int len = value.size();
    for (int i = 0; i < len; ++i) {
      unsigned char c = value[i];
      if (isalpha(c) || (c >= '0' && c <= '9') || c == '-' || c == '_' ||
          c == '.' || c == '~') {
        message->append(1, c);
      } else {
        char buf[4];
        snprintf(buf, 4, "%%%02x", c);
        message->append(buf);
      }
    }
  }

  string host_;
  int16_t port_;
  string path_;
  string info_hash_;
  int local_port_;
  CBThread thread_;
  ythread::Mutex mu_;
  vector<ypeer::Peer> peers_;
  bool shutdown_;
  bool error_;
};

Peers* NewPeers(const std::string& tracker_url,
                const std::string& info_hash,
                int16_t local_port) {
  assert(info_hash.size() == 20);
  yhttpserver::URL url(tracker_url);
  if (!url.IsValid()) {
    return NULL;
  }
  assert(url.User().empty());
  assert(url.Password().empty());
  assert(url.Scheme() == "http");
  assert(url.Query().empty());
  return new PeersImpl(url.Host(), url.Port(), url.Path(), info_hash,
                       local_port);
}

}  // namespace ypeer
