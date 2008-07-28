// peers.cpp
// Author: Allen Porter <allen@thebends.org>

#include "peers.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <err.h>
#include <sysexits.h>
#include <ythread/callback-inl.h>
#include <ythread/condvar.h>
#include <ythread/mutex.h>
#include <ythread/thread.h>
#include "response.h"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

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
  PeersImpl(CFURLRef url,
            const string& info_hash,
            int16_t local_port)
      : base_url_(url),
        info_hash_(info_hash),
        local_port_(local_port),
        thread_(ythread::NewCallback(this, &PeersImpl::StartInThread)),
        shutdown_(false),
        error_(false),
        connected_(false),
        callback_(NULL) {
    thread_.Start();
  }

  virtual ~PeersImpl() {
    mu_.Lock();
    shutdown_ = true;
    mu_.Unlock();
    CFRelease(base_url_);
    thread_.Join();
  }

  bool Error() {
    mu_.Lock();
    bool e = error_;
    mu_.Unlock();
    return e;
  }

  string ErrorString() {
    mu_.Lock();
    string s = error_string_;
    mu_.Unlock();
    return s;
  }

  virtual bool Connected() {
    mu_.Lock();
    bool c = connected_;
    mu_.Unlock();
    return c;
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
    mu_.Lock();
    callback_ = callback;
    mu_.Unlock();
  }

 private:
  void SetError(const string& error_string) {
    mu_.Lock();
    error_ = true;
    error_string_ = error_string;
    mu_.Unlock();
  }

  string GetPortString() {
    char buf[BUFSIZ];
    sprintf(buf, "%d", local_port_);
    return string(buf);
  }

  CFHTTPMessageRef BuildRequest(const map<string, string>& params) {
    // Prepare the query parameters
    CFMutableDataRef query = CFDataCreateMutable(kCFAllocatorDefault, 0);
    BuildQuery(params, query);

    // Append the query parameters to the base URL
    CFURLRef url = CFURLCreateWithBytes(kCFAllocatorDefault,
                                        CFDataGetBytePtr(query),
                                        CFDataGetLength(query),
                                        kCFStringEncodingUTF8,
                                        base_url_);
    CFHTTPMessageRef request =
        CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("GET"),
                                   url, kCFHTTPVersion1_1);
    CFHTTPMessageSetHeaderFieldValue(request, CFSTR("User-Agent"),
        CFSTR("libypeer"));
    CFRelease(query);
    CFRelease(url);
    return request;
  }

  bool ReadStream(CFReadStreamRef stream, CFMutableDataRef data) {
    do {
      uint8 buf[1024];
      CFStreamStatus stream_status = CFReadStreamGetStatus(stream);
      if (stream_status == kCFStreamStatusAtEnd) {
        return true;
      }
      if (stream_status != kCFStreamStatusOpen) {
        cerr << "Stream status: " << stream_status;
        return false;
      }
      CFIndex bytes_read = CFReadStreamRead(stream, buf, 1024);
      if (bytes_read < 0) {
        // TODO(allen): CFReadStremGetError to get error detail
        cerr << "Unable to read from stream";
        return false;
      } else if (bytes_read == 0) {
        return true;
      }
      CFDataAppendBytes(data, buf, bytes_read);
    } while (true);
  }

  void StartInThread() {
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

      CFHTTPMessageRef request = BuildRequest(params);
      CFReadStreamRef stream =
          CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, request);
      CFRelease(request);
      if (!CFReadStreamOpen(stream)) {
        cerr << "Unable to open HTTP stream" << endl;
        SetError("Unable to open HTTP stream");
        return;
      }
      CFMutableDataRef body_data = CFDataCreateMutable(kCFAllocatorDefault, 0);
      if (!ReadStream(stream, body_data)) {
        cerr << "Error while reading stream" << endl;
        SetError("Error while reading stream");
        return;
      }

      CFHTTPMessageRef response =
          (CFHTTPMessageRef)CFReadStreamCopyProperty(
              stream,
              kCFStreamPropertyHTTPResponseHeader);
      if (response == NULL) {
        cerr << "Error obtaining HTTP response" << endl;
        SetError("Error obtaining HTTP response");
        return;
      }

      if (!CFHTTPMessageIsHeaderComplete(response)) {
        cerr << "HTTP Header incomplete" << endl;
        SetError("HTTP Header incomplete");
        return;
      }

      CFIndex status = CFHTTPMessageGetResponseStatusCode(response);
      if (status != 200) {
        cerr << "Unexpected HTTP response status: " << status << endl;
        SetError("Unexpected HTTP response status");
        return;
      }
/*
      CFDataRef body_data = CFHTTPMessageCopyBody(response);
      if (body_data == NULL) {
        cerr << "Error creating response body" << endl;
        SetError("Error creating response body");
        return;
      }
*/

      string body((const char*)CFDataGetBytePtr(body_data), CFDataGetLength(body_data));

      CFRelease(response);
      CFRelease(body_data);

      ResponseMessage result;
      if (!ParseResponseMessage(body, &result)) {
        cerr << "Failed to parse tracker response" << endl;
        SetError("Failed to parse tracker response");
        return;
      }
      if (!result.failure_reason.empty()) {
        cerr << "Failure from tracker: " << result.failure_reason << endl;
        SetError("Failure from tracker");
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
      error_ = false;
      error_string_.clear();
      connected_ = true;
      peers_ = result.peers;
      ythread::Callback* cb = callback_;
      mu_.Unlock();
      // Don't invoke the callback with the lock held, since likely the
      // caller will invoke GetPeers()
      if (cb != NULL) {
        cb->Execute();
      }

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

  void BuildQuery(const map<string, string>& params, CFMutableDataRef message) {
    int i = 0;
    CFDataAppendBytes(message, (const UInt8*)"?", 1);
    for (map<string, string>::const_iterator it = params.begin();
         it != params.end(); ++it) {
      if (i != 0) {
        CFDataAppendBytes(message, (const UInt8*)"&", 1);
      }
      EscapeAndAppend(it->first, message);
      CFDataAppendBytes(message, (const UInt8*)"=", 1);
      EscapeAndAppend(it->second, message);
      i++;
    }
  }

  void EscapeAndAppend(const string& value, CFMutableDataRef message) {
    int len = value.size();
    for (int i = 0; i < len; ++i) {
      unsigned char c = value[i];
      if (isalpha(c) || (c >= '0' && c <= '9') || c == '-' || c == '_' ||
          c == '.' || c == '~') {
        CFDataAppendBytes(message, (const UInt8*)&c, 1);
      } else {
        char buf[4];
        snprintf(buf, 4, "%%%02x", c);
        CFDataAppendBytes(message, (const UInt8*)buf, 3);
      }
    }
  }

  CFURLRef base_url_;
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
  string error_string_;
  bool connected_;
  ythread::Callback* callback_;
};

Peers* NewPeers(const std::string& tracker_url,
                const std::string& info_hash,
                int16_t local_port) {
  assert(info_hash.size() == 20);
  CFURLRef url = CFURLCreateWithBytes(kCFAllocatorDefault,
                                      (const UInt8*)tracker_url.c_str(),
                                      tracker_url.size(),
                                      kCFStringEncodingUTF8, NULL);
  if (url == NULL) {
    return NULL;
  }
  return new PeersImpl(url, info_hash, local_port);
}

}  // namespace ypeer
