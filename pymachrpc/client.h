// client.h
//
// A very simple C++ wrapper around mach_msg that also performs bootstrap
// service lookups for a particular service name.

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <mach/mach.h>
#include <string>
#include <vector>

namespace machrpc {

// Abstract Client interface
class Client {
 public:
  virtual ~Client();

  // Sends a blocking message.  This is a simple wrapper around mach_msg that
  // handles filling in the headers.
  virtual std::string SendMessage(
      mach_msg_id_t msgh_id,
      const std::string& request,
      int reply_size) = 0;

  // For invoking a message with no response
  virtual void SendMessage(
      mach_msg_id_t msgh_id,
      const std::string& request) = 0;

 protected:
  Client();
};

// Creates a Client object connected to the specified service name.
Client* NewClient(const std::string& service_name);

// Creates a client for the boostrap server.  See bootstrap.defs
Client* NewBootstrapClient();

// SWIG exception handling routines
void ClearException();
char* CheckException();

}  // namespace machrpc

#endif  // __CLIENT_H__
