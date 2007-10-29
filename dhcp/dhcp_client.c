#include <err.h>
#include <sysexits.h>
#include <libnet.h>
#include "messages.h"
#include "options.h"

static u_char broadcast_mac[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static u_char source_mac[] = { 0x00, 0x0a, 0x95, 0xbe, 0x88, 0xc6 };

#define MAX_DHCP_SIZE (1500 - LIBNET_IPV4_H - LIBNET_UDP_H)

// TODO: Allow libnet/pcap mode or socket mode

int main(int argc, char* argv[]) {
  srandomdev();
  if (argc != 2) {
    errx(EX_USAGE, "Usage: %s <device>", argv[0]);
  }
  char* dev = argv[1];
  char errbuf[LIBNET_ERRBUF_SIZE];
  libnet_t* context = libnet_init(LIBNET_LINK, dev, errbuf);
  if (context == NULL) {
    errx(EX_UNAVAILABLE, "libnet_init: %s", errbuf);
  }

  uint8_t buf[MAX_DHCP_SIZE];
  uint16_t size = MAX_DHCP_SIZE;
  build_discover(source_mac, "lozenge", buf, &size);

  libnet_ptag_t ether_tag = LIBNET_PTAG_INITIALIZER;
  libnet_ptag_t ip_tag = LIBNET_PTAG_INITIALIZER;
  libnet_ptag_t udp_tag = LIBNET_PTAG_INITIALIZER;

  udp_tag = libnet_build_udp(68, 67, LIBNET_UDP_H + size,
                            0, buf, size,
                            context, udp_tag);

  u_short ipid = random() & 0xffff;
  ip_tag = libnet_build_ipv4(
               LIBNET_IPV4_H + LIBNET_UDP_H + size,
               0, ipid, 0, 0xff,
               IPPROTO_UDP, 0, 0, 0xffffffff,
               NULL, 0, context, ip_tag);

  ether_tag = libnet_build_ethernet(broadcast_mac, source_mac,
                                    ETHERTYPE_IP, NULL, 0, context, ether_tag);

  if (libnet_write(context) == -1) {
    libnet_destroy(context);
    errx(EX_UNAVAILABLE, "libnet_write");
  }
  printf("Wrote\n");
  libnet_destroy(context);

  return 0;
}
