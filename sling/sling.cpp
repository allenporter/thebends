// sling.cpp
// Author: Allen Porter <allen@thebends.org>
//
// More background:
// http://allenporter.tumblr.com/post/6957276932/upstream-bandwidth

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sysexits.h>
#include <stdio.h>
#include <unistd.h>
#include <gflags/gflags.h>
#include <map>

DEFINE_string(ip, "", "Destination IP address");
DEFINE_int32(ttl, 1, "Number of hops");
DEFINE_int32(max_outstanding, 50, "Maximum number of outstanding packets");
DEFINE_int32(payload_size, 1024, "Size of UDP payload");
DEFINE_bool(debug, false, "Enable extra debugging");

using ::std::map;

static const int kStatsTimerId = 0x100;
static const int kStatsTimeoutMs = 1000;  // 1s
static const int kExpireTimerId = 0x101;
static const int kExpireTimeoutMs = 10;

static const int kDeadlineMs = 5000;

// Number of kevents to fetch at once.  Larger than the number of event types
// that we register interest in so that we might be able to fetch them all at
// once.
// If we fetch fewer, is it possible we will starve? Try setting this to 1 and
// see if the code can be simplified.
static const int kMaxEvents = 10;

static const int kRecvSize = 1024;

// TODO
static const int kOverheadSize = 0;

void SetSockOpt(int sock, int level, int opt, int value) {
  if (setsockopt(sock, level, opt, (char *)&value, sizeof(value)) < 0) {
    perror("setsockopt");
    exit(EX_OSERR);
  }
}

void SetSockOpt(int sock, int level, int opt, bool enabled) {
  int on = enabled ? 1 : 0;
  SetSockOpt(sock, level, opt, on);
}

u_int64_t NowInMs() {
  struct timeval tp;
  if (gettimeofday(&tp, NULL) == -1) {
    perror("gettimeofday");
    exit(EX_OSERR);
  }
  return tp.tv_sec * 1000 + tp.tv_usec / 1000; 
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, false);

  struct sockaddr_in to;
  bzero(&to, sizeof(struct sockaddr_in));
  to.sin_family = AF_INET;
  to.sin_addr.s_addr = inet_addr(FLAGS_ip.c_str());
  printf("Sending to %s w/ TTL of %d\n", inet_ntoa(to.sin_addr), FLAGS_ttl);

  int icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (icmp_sock == -1) {
    perror("socket");
    return EX_OSERR;
  }
  int udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (udp_sock == -1) {
    perror("socket");
    return EX_OSERR;
  }

  SetSockOpt(udp_sock, IPPROTO_IP, IP_TTL, FLAGS_ttl);
  if (FLAGS_debug) {
    SetSockOpt(icmp_sock, SOL_SOCKET, SO_DEBUG, true);
    SetSockOpt(udp_sock, SOL_SOCKET, SO_DEBUG, true);
  }

  int kq = kqueue();

  struct kevent kev;

  EV_SET(&kev, icmp_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
  int ret = kevent(kq, &kev, 1, NULL, 0, NULL);
  if (ret == -1) {
    perror("kevent");
    return EX_OSERR;
  }

  EV_SET(&kev, udp_sock, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
  ret = kevent(kq, &kev, 1, NULL, 0, NULL);
  if (ret == -1) {
    perror("kevent");
    return EX_OSERR;
  }

  EV_SET(&kev, kStatsTimerId, EVFILT_TIMER, EV_ADD, 0, kStatsTimeoutMs, NULL);
  ret = kevent(kq, &kev, 1, NULL, 0, NULL);
  if (ret == -1) {
    perror("kevent");
    return EX_OSERR;
  }

  EV_SET(&kev, kExpireTimerId, EVFILT_TIMER, EV_ADD, 0, kExpireTimeoutMs, NULL);
  ret = kevent(kq, &kev, 1, NULL, 0, NULL);
  if (ret == -1) {
    perror("kevent");
    return EX_OSERR;
  }

  u_int64_t bytes_sent = 0;
  u_int64_t packets_sent = 0;

  u_int16_t seq = 4040;
  uint8_t packet[FLAGS_payload_size];
  bzero(packet, FLAGS_payload_size);
  for (int i = 0; i < FLAGS_payload_size; ++i) {
    packet[i] = 'A' + i % 26;
  }

  map<u_int64_t, u_int64_t> outstanding;

  int expired = 0;
  while (true) {
    struct kevent events[kMaxEvents];
    int event_count = kevent(kq, NULL, 0, events, kMaxEvents, NULL);
    if (event_count == -1) {
      perror("kevent");
      return EX_OSERR;
    }

    u_int64_t now = NowInMs();
    for (int i = 0; i < event_count; ++i) {
      switch (events[i].filter) {
        case EVFILT_TIMER: {
          int timer_id = events[i].ident;
          if (timer_id != kExpireTimerId &&
              timer_id != kStatsTimerId) {
            fprintf(stderr, "Unexpected timer id: %d\n", timer_id);
            return EX_SOFTWARE;
          }
          if (timer_id == kExpireTimerId) {
            // erase any rpcs that were outstanding past their deadline
            map<u_int64_t, u_int64_t>::iterator it = outstanding.begin();
            while (it != outstanding.end()) {
              map<u_int64_t, u_int64_t>::iterator pos = it++;
              if (pos->second < now) {
                outstanding.erase(pos);
                expired++;
              } 
            }
          } else {
            // Dump stats
            printf("Status: %lld packets/s %lld bytes/s %lu out %d expired\n",
                   packets_sent, bytes_sent, outstanding.size(), expired);
            fflush(stdout);
            packets_sent = 0;
            bytes_sent = 0;
            expired = 0;
          }
          break;
        }
        case EVFILT_READ:
          if (outstanding.size() > 0) {
            u_int8_t recv_buffer[kRecvSize];
            struct sockaddr_in from;
            socklen_t from_len = sizeof(struct sockaddr_in);
            ssize_t recv_size = recvfrom(icmp_sock, recv_buffer, kRecvSize, 0,
               (struct sockaddr*)&from, &from_len);
            if (recv_size == -1) {
              perror("recvfrom");
              return EX_OSERR;
            } else if ((size_t)recv_size < sizeof(u_int64_t)) {
              break;
            } else if ((size_t)recv_size < (ICMP_MINLEN +
                            sizeof(struct ip) * 2 + sizeof(struct udphdr))) {
              // short packet
              break;
            }
            struct ip* iphdr = (struct ip*)recv_buffer;
            struct icmp* icp = (struct icmp*)(iphdr + 1);
            if (icp->icmp_type != ICMP_TIMXCEED ||
                icp->icmp_code != ICMP_TIMXCEED_INTRANS) {
              break;
            }
            struct ip* hip = &icp->icmp_ip;
            if (hip->ip_p != IPPROTO_UDP) {
              break;
            }
            struct udphdr* udp = (struct udphdr*)(hip + 1);
            u_int16_t seq = ntohs(udp->uh_dport);
            map<u_int64_t, u_int64_t>::iterator it = outstanding.find(seq);
            if (it == outstanding.end()) {
              fprintf(stderr, "Got unexpected packet for id %d\n", seq);
              break;
            }
            outstanding.erase(it);
            packets_sent++;
            bytes_sent += FLAGS_payload_size + kOverheadSize;
          }
          break;
        case EVFILT_WRITE:
          if (outstanding.size() < (size_t)FLAGS_max_outstanding) {
            seq++;
            outstanding[seq] = now + kDeadlineMs;
            to.sin_port = htons(seq);
            int nsent = sendto(udp_sock, packet, FLAGS_payload_size,
                0, (const struct sockaddr*)&to, sizeof(sockaddr_in));
            if (nsent == -1) {
              perror("sendto");
              return EX_OSERR;
            } else if (nsent != FLAGS_payload_size) {
              fprintf(stderr, "Got %d, expected %d from sendto", nsent,
                  FLAGS_payload_size);
              return EX_SOFTWARE;
            }
          }
          break;

        default:
          fprintf(stderr, "Unexpected ident code: %d\n", events[i].ident); 
          return EX_SOFTWARE;
      }
    }
  }

  close(kq);
  close(icmp_sock);
  close(udp_sock);
}
