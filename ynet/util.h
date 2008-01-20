// util.h
// Author: Allen Porter <allen@thebends.org>

#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

namespace ynet {

struct sockaddr_in SockAddr(const char* ip, int16_t port);

void SetNonBlocking(int sock);

}  // namespace ynet
