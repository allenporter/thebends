// response.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __YPEER_RESPONSE_H__
#define __YPEER_RESPONSE_H__

#include <string>
#include <vector>
#include "peers.h"

namespace ypeer {

struct ResponseMessage {
  ResponseMessage() : interval(0),
                      min_interval(0),
                      complete(0),
                      incomplete(0) { }

  std::string failure_reason;
  std::string warning_message;
  int interval;
  int min_interval;
  std::string tracker_id;
  int complete;
  int incomplete;
  std::vector<Peer> peers;
};

bool ParseResponseMessage(const std::string& message,
                          ResponseMessage* response);

}  // namespace ypeer

#endif  // __YPEER_RESPONSE_H__
