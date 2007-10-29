/*
 * gutterspeak.c
 *
 * howto:
 * $ gcc -o gutterspeak gutterspeak.c -lpcap
 * $ ./gutterspeak en1
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <pcap.h>	
#include <assert.h>

#define SNAPLEN 65535
#define READ_TIMEOUT 500
#define ETHER_NTOA(a) (ether_ntoa((struct ether_addr *)&(a)))
#define INET_NTOA(a) (inet_ntoa(*(struct in_addr *)(&a)))

/*
 * pcap session. this is global just so that the signal handler can break the
 * loop properly
 */
pcap_t* handle;

/*
 * returns 0 if the packet is correctly formatted, and < 0 if not.
 */
int can_parse(const struct pcap_pkthdr *header,
              const struct ether_header* ethr_hdr,
              const struct tcpiphdr* tcp_hdr) {
  if (header->caplen != header->len) {
    fprintf(stderr, "Ingoring caplen != len");
    return -1;
  }
  if (ethr_hdr->ether_type != ETHERTYPE_IP) {
    fprintf(stderr, "Ignoring ether_type != ETHERTYPE_IP");
    return -1;
  }
  if (tcp_hdr->ti_i.ih_pr == 4) {
    fprintf(stderr, "Ignoring protocol != 4");
    return -1;
  }
  int header_len = sizeof(struct ether_header) + sizeof(struct tcpiphdr);
  if (tcp_hdr->ti_i.ih_len == (header->len - header_len)) {
    fprintf(stderr, "Ignoring packet of incorrect length");
    return -1;
  }
  if (tcp_hdr->ti_t.th_dport == 3724) {
    // ignore outgoingpackets
    return -1;
  }
  if (tcp_hdr->ti_t.th_sport != 3724) {
    fprintf(stderr, "Ignoring port != 3724");
    return -1;
  }
  return 0;
}

void dump_hex(const u_char* payload, int len) {
  const u_char *end = payload + len;
  const u_char *c = payload;
  while (c != end) {
    printf("%02x|", *c);
    ++c;
  }
  printf("\n\n");
}

void dump_ascii(const u_char* payload, int len) {
  const u_char *end = payload + len;
  const u_char *c = payload;
  while (c != end) {
    if (*c == '\n') {
      printf("\\n |");
    } else {
      printf(" %c |", *c);
    }
    ++c;
  }
  printf("\n\n");
}

void dump_string(const u_char* payload, int len) {
  const u_char *end = payload + len;
  const u_char *c = payload;
  const u_char* first_ascii = NULL;
  int state = 0;
  // TODO: I have no idea what the protocol here is, this just seemed to find
  // interestsing strings.
  while (c <= end) {
    if (c < (end - 3) &&
        c[0] == 0x00 && c[1] == 0x00 && c[2] == 0x00 && isascii(c[3])) {
      state = 1;
      c += 3;
      first_ascii = c;
    } else if (state == 1) {
      if (*c == 0x00 && strlen((char*)first_ascii) > 2) {
        printf("[%s]\n", (char*)first_ascii);
        return;
      } else if (!isascii(*c) || *c < ' ' || *c > '~') {
        state = 0;
        first_ascii = NULL;
      }
      ++c;
    } else {
      ++c;
    }
  }
}

/*
 * callback invoked by pcap run when a packet arrives
 */
void got_packet(u_char *args, struct pcap_pkthdr *header,
                u_char *packet) {
  struct ether_header* ethr_hdr;
  struct tcpiphdr* tcp_hdr;
  u_char* payload;

  ethr_hdr = (struct ether_header*)packet;
  tcp_hdr = (struct tcpiphdr*)(ethr_hdr + 1);
  payload =  (u_char*)(tcp_hdr + 1);
  int payload_len = header->len - (sizeof(struct ether_header) +
                                   sizeof(struct tcpiphdr));

  if (payload_len > 0 && can_parse(header, ethr_hdr, tcp_hdr) < 0) {
    return;
  }

  if (payload[0] != 0x01 || payload[1] != 0x01 ||
      payload[2] != 0x08 || payload[3] != 0x0a) {
    // Common pattern, for some reason
    return;
  }
  dump_string(payload, payload_len);
}

void signal_handler(int signal) {
  assert(SIGINT == signal);
  printf("SIGINT received; breaking pcap loop\n");
  pcap_breakloop(handle);
}

pcap_t* setup_pcap(const char* dev) {
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program bpf;          /* stores the filter */
  char filter[] = "port 3724";     /* text version of filter */
  struct in_addr addr;
  bpf_u_int32 mask;
  bpf_u_int32 net;
  char *net_s;
  char *mask_s;

  /* sniff promiscuosly until an error occurs */
  handle = pcap_open_live(dev, SNAPLEN, 1, READ_TIMEOUT, errbuf);
  if (handle == NULL) {
    fprintf(stderr, "%s\n", errbuf);
    return NULL;
  }

  memset(&bpf, 0, sizeof(struct bpf_program));

  /* return the IP and netmask of the device */
  if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
    fprintf(stderr, "%s\n", errbuf);
    return NULL;
  }
  
  addr.s_addr = net;
  net_s = inet_ntoa(addr);
  if (net_s == NULL) {
    fprintf(stderr, "inet_ntoa");
    return NULL;
  }
  printf("NET: %s\n", net_s);

  addr.s_addr = mask;
  mask_s = inet_ntoa(addr);
  if (mask_s == NULL) {
    fprintf(stderr, "inet_ntoa");
    return NULL;
  }
  printf("MASK: %s\n", mask_s);

  /* complie and set the filter */
  pcap_compile(handle, &bpf, filter, 0, net);
  pcap_setfilter(handle, &bpf);

  return handle;
}

int main(int argc, char *argv[]) {
  char *dev = argv[1];
  if (argc != 2) {
    printf("Usage: %s <device>\n", argv[0]);
    exit(0);
  }

  if (getuid() != 0) {
    fprintf(stderr, "You must be root to use this program.\n");
    exit(1);
  }

  handle = setup_pcap(dev);
  if (handle == NULL) {
    fprintf(stderr, "Unable to setup pcap; exiting");
    exit(1);
  }
  printf("Listening on device: %s\n", dev);

  signal(SIGINT, signal_handler);
  /* capture packets forever! */
  if (pcap_loop(handle, 0, (pcap_handler)got_packet, NULL) == -1) {
    fprintf(stderr, "pcap_loop: %s", pcap_geterr(handle));
    exit(1);
  }

  pcap_close(handle);
  return 0;
}
