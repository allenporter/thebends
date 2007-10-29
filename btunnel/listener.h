// listener.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __BTUNNEL_LISTENER_H__
#define __BTUNNEL_LISTENER_H__


#include <netinet/in.h>
#include <sys/types.h>
#include <ythread/thread.h>
#include <ythread/callback.h>

namespace btunnel {

static const int kPacketBufferSize = 1500;

// Represents information about a captured packet.
struct packet_info {
  struct sockaddr_in sender;
  unsigned char data[kPacketBufferSize];
  size_t length;
};

// The packet_info argument is only valid for the duration of the callback and
// the invoked callback should not keep a pointer to it.
typedef ythread::Callback1<struct packet_info*> ReceivedCallback;

// Creates a new background thread that captures packets from the specified
// socket.  The ReceivedCallback is invoked from the background thread, and the
// callback function should not block.  The caller is reponsible for deleing
// the supplied ReceivedCallback.
ythread::Thread* NewSocketListenerThread(int sock, ReceivedCallback* cb);

}  // namespace btunnel

#endif  // __BTUNNEL_LISTENER_H__
