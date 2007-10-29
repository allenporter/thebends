/*
 * btunnel.
 * Copyright (c) 2005 Allen Porter <allen@thebends.org>
 *
 * btunnel is a utility to tunnel local bonjour traffic to a remote network.
 * NOTE: Not tested and probably does not work.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

#define SNAPLEN 65535           /* maximum number of bytes captured */
#define READ_TIMEOUT 500
#define INADDR_MULTICAST_BONJOUR 0xEE0000FB

#define ETHER_NTOA(a) (ether_ntoa((struct ether_addr *)&(a)))
#define INET_NTOA(a) (inet_ntoa(*(struct in_addr *)(&a)))

#define MDNS_PORT 5353
#define TUNNEL_PORT 9940
#define PBUFSIZ 1500

int s_mcast;			/* multicast socket */
int s_tunnel;			/* tunnel socket */
struct in_addr addr_tunnel;	/* remote tunnel endpoint */
struct sockaddr_in sin_tunnel;
struct sockaddr_in sin_mcast;

void cleanup(int sig);
void usage(char *arg);
void init_args(int argc, char * const argv[]);
void dump_packet(u_char * buf, int size);
void handle_tunnel_packet();
void handle_mcast_packet();
void init_multicast();
void init_tunnel();
void teardown();

void cleanup(int sig) {
  fprintf(stderr, "Interrupted...\n");
  teardown();
  exit(sig);
}

void usage(char * arg) {
  printf("usage: %s -t <remote tunnel ip>\n", arg);
  exit(0);
}

void init_args(int argc, char * const argv[]) {
  char ch;
  int dest = 0;
  while ( (ch = getopt(argc, argv, "t:")) != EOF) {
    switch (ch) {
        case 't':
          if ( inet_aton(optarg, &addr_tunnel) == 0 ) {
            fprintf(stderr, "Bad destination: %s",
                    optarg);
            exit(1);
          }
          dest++;
          break;
        case '?':
          default:
          usage(argv[0]);
          break;
      }
  }
  if (!dest) {
    usage(argv[0]);
  }
}

void dump_packet(u_char * buf, int size) {
  int i;
  for ( i = 0; i < size; ++i ) {
    printf("%x ", buf[i]);
  }
  printf("\n");
}

void init_tunnel() {
  struct sockaddr_in sin;
  unsigned int opt = 1;

  if ( (s_tunnel = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 ) {
    perror("socket (tunnel)");
    exit(1);
  }

  if ( setsockopt(s_tunnel, SOL_SOCKET, SO_REUSEPORT, &opt,
                  sizeof(opt)) == -1 ) {
    perror("setsockopt (tunnel)");
    exit(EXIT_FAILURE); 
  }

  bzero(&sin, sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(TUNNEL_PORT);

  if ( bind(s_tunnel, (struct sockaddr *)&sin,
            sizeof(struct sockaddr_in)) == -1 ) {
    perror("bind (tunnel)");
    exit(1);
  }

  /* endpoint */
  bzero(&sin_tunnel, sizeof(struct sockaddr_in));
  sin_tunnel.sin_family = AF_INET;
  sin_tunnel.sin_addr.s_addr = htonl(addr_tunnel.s_addr);
  sin_tunnel.sin_port = htons(TUNNEL_PORT);
}

void init_multicast() {
  struct sockaddr_in sin;
  struct ip_mreq mcast_cmd;
  unsigned int opt = 1;
  unsigned char ttl = 1;

  if ( (s_mcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 ) {
    perror("socket (mcast)");
    exit(1);
  }

  if ( setsockopt(s_mcast, SOL_SOCKET, SO_REUSEPORT, &opt,
                  sizeof(opt)) == -1 ) {
    perror("setsockopt (mcast)");
    exit(EXIT_FAILURE); 
  }

  if ( setsockopt(s_mcast, IPPROTO_IP, IP_MULTICAST_LOOP, &opt,
                  sizeof(opt)) < 0 ) {   
    perror("setsockopt:IP_MULTICAST_LOOP (mcast)"); 
    exit(EXIT_FAILURE); 
  }

  if ( setsockopt(s_mcast, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl,
                  sizeof(ttl)) < 0 ) {   
    perror("setsockopt:IP_MULTICAST_TTL (mcast)"); 
    exit(EXIT_FAILURE); 
  }

  /* multicast group */
  bzero(&sin_mcast, sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(MDNS_PORT);

  mcast_cmd.imr_multiaddr.s_addr = htonl(INADDR_MULTICAST_BONJOUR);
  mcast_cmd.imr_interface.s_addr = htonl(INADDR_ANY);
  if ( setsockopt(s_mcast, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast_cmd,
                  sizeof(mcast_cmd)) < 0 ) {
    perror("setsockopt:IP_ADD_MEMBERSHIP (mcast)"); 
    exit(EXIT_FAILURE); 
  }

  if ( bind(s_mcast, (struct sockaddr *)&sin,
            sizeof(struct sockaddr_in)) == -1 ) {
    perror("bind (mcast)");
    exit(1);
  }

  bzero(&sin_mcast, sizeof(struct sockaddr_in));
  sin_mcast.sin_family = AF_INET;
  sin_mcast.sin_addr.s_addr = htonl(INADDR_MULTICAST_BONJOUR);
  sin_mcast.sin_port = htons(MDNS_PORT);
}

void teardown() {
  struct ip_mreq mcast_cmd;

  mcast_cmd.imr_multiaddr.s_addr = htonl(INADDR_MULTICAST_BONJOUR);
  mcast_cmd.imr_interface.s_addr = htonl(INADDR_ANY);

  if ( setsockopt(s_mcast, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mcast_cmd,
                  sizeof(mcast_cmd)) < 0 ) {
    perror("setsockopt:IP_DROP_MEMBERSHIP");
    exit(EXIT_FAILURE); 
  }

  close(s_mcast);
  close(s_tunnel);
}

// a hack to mark this packet as one that btunnel has touched. set the first
// byte to 1.
// TODO: This is ghetto
void mark_packet(u_char * buf, int bufsiz) {
  if (bufsiz > 13) {
    buf[13] = 'Z';
  } else {
    fprintf(stderr, "Can't mark packet; too small :/");
    exit(1);
  }
}

int is_marked_packet(u_char * buf, int bufsiz) {
  return ((bufsiz > 13) ? (buf[13] == 'Z') : 0);
}

void handle_mcast_packet() {
  ssize_t bufsiz;
  ssize_t sent;
  u_char buf[PBUFSIZ];
  struct sockaddr_in sin;
  u_int sin_len;

  if ( (bufsiz = recvfrom(s_mcast, buf, PBUFSIZ, 0, (struct sockaddr *)&sin,
                          &sin_len)) == -1 ) {
    perror("recvfrom");
    exit(1);
  }

  if ( is_marked_packet(buf, bufsiz) ) {
    printf(" [ Ignored packet ]\n");
    return;
  }

  printf("Multicast packet received\n");

  sent = sendto(s_tunnel, buf, bufsiz, 0, /* flags */
                (struct sockaddr *)&sin_tunnel, sizeof(struct sockaddr_in));

  if (sent != bufsiz) {
    fprintf(stderr, "packet size didn't match\n");
  }
}

void handle_tunnel_packet() {
  ssize_t bufsiz;
  ssize_t sent;
  u_char buf[PBUFSIZ];
  struct sockaddr_in sin;
  u_int sin_len;

  printf("Tunnel packet received\n");
  if ( (bufsiz = recvfrom(s_tunnel, buf, PBUFSIZ, 0, (struct sockaddr *)&sin,
                          &sin_len)) == -1 ) {
    perror("recvfrom");
    exit(1);
  }

  mark_packet(buf, bufsiz);

  /* make sure sin matches sin_tunnel ? -- how secure */

  sent = sendto(s_mcast, buf, bufsiz, 0, /* flags */
                (struct sockaddr *)&sin_mcast, sizeof(struct sockaddr_in));

  if (sent != bufsiz) {
    fprintf(stderr, "packet size didn't match\n");
  }
}


void loop() {
  fd_set rfds;
  int fd_max;

  fd_max = (s_tunnel > s_mcast) ? s_tunnel : s_mcast;

  for (;;) {
    FD_ZERO(&rfds);
    FD_SET(s_tunnel, &rfds);
    FD_SET(s_mcast, &rfds);

    select(fd_max+1, &rfds, NULL, NULL, NULL);
    if (FD_ISSET(s_tunnel, &rfds)) {
      handle_tunnel_packet();
    }
    if (FD_ISSET(s_mcast, &rfds)) {
      handle_mcast_packet();
    }
  }
}

int main(int argc, char* argv[]) {
  init_args(argc, argv);
  init_multicast();
  init_tunnel();
  loop();
  teardown(s_mcast);
  printf("done!\n");
  return 0;
}
