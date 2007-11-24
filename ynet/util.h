// util.h
// Author: Allen Porter <allen@thebends.org>

#include <sys/types.h>

namespace ynet {

struct sockaddr_in SockAddr(const char* ip, int16_t port);

void SetNonBlocking(int sock);

}  // namespace ynet
