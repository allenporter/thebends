/*
 * arpdropper.c - Create custom ARP packets and drop them on the wire
 * allen porter (allen@thebends.org)
 * arpdropper was written as an exercise for learning libnet. If you have
 * any optimizations or additions to this program, please e-mail them to me.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <libnet.h>
#include <libnet/libnet-headers.h>
#include <libnet/libnet-functions.h>

#define MACADDR_LEN 6
#define IPADDR_LEN 4

struct arp_payload {
	struct ether_addr ar_sha;	/* sender hardware address */
	struct in_addr ar_spa;			/* sender protocol address */
	struct ether_addr ar_tha;	/* target hardware address */
	struct in_addr ar_tpa;			/* target protocol address */
};

u_char broadcast_mac[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
u_char empty_mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
void usage(char *argv[]) {
	fprintf(stderr, "Usage: %s -i <device> -s <source ip> -d <dest ip>\n",
			argv[0]);
	fprintf(stderr, "For arp replies:\n");
	fprintf(stderr, "       %s -r -i <device> -s <source ip>"
			" -m <source mac> -d <dest ip>\n",
			argv[0]);
}

int main(int argc, char *argv[]) {
	char errbuf[LIBNET_ERRBUF_SIZE];
	libnet_ptag_t ether_tag = LIBNET_PTAG_INITIALIZER;
	libnet_ptag_t arp_tag = LIBNET_PTAG_INITIALIZER;
	libnet_t * context;
	struct arp_payload request;
	int numbytes;				/* bytes written */
	char * source_ip = NULL;
	char * source_mac = NULL;
	char * dest_ip = NULL;
	char * dev = NULL;
	char optch;	/* getopt */
	int reply = 0;

	/* what can we stick in a payload that is interesting? */
	u_int8_t * payload = NULL;
	u_int8_t payload_len = 0;

	while ((optch = getopt(argc, argv, "rm:i:s:d:")) != EOF) {
		switch (optch) {
			case 'r':
				reply = 1;
				break;
			case 'i':
				dev = strdup(optarg);
				break;
			case 's':
				source_ip = strdup(optarg);
				break;
			case 'd':
				dest_ip = strdup(optarg);
				break;
			case 'm':
				source_mac = strdup(optarg);
		}
	}

	if (dev == NULL || source_ip == NULL || dest_ip == NULL) {
		usage(argv);
		exit(1);
	}
	if (reply == 1 && source_mac == NULL) {
		usage(argv);
		exit(1);
	}

	context = libnet_init(LIBNET_LINK, dev, errbuf);
	if (context == NULL) {
		fprintf(stderr, "Unable to initialize library: %s",
				errbuf);
		exit(1);
	}

	if (reply) {
		struct ether_addr * s =
			(struct ether_addr *)ether_aton(source_mac);
		memcpy(&request.ar_sha, s, sizeof(struct ether_addr));	
	} else {
		/* TODO - this may fail, check the return call of get_hwaddr */
		memcpy(&request.ar_sha, libnet_get_hwaddr(context),
			sizeof(struct ether_addr));
	}

	/* convert source IP from string */
	if (inet_aton(source_ip, &request.ar_spa) == 0) {
		fprintf(stderr, "Invalid Source IP Address\n");
		libnet_destroy(context);
		exit(1);
	}

	/* set the destination mac as an empty address */	
	memcpy(&request.ar_tha, (struct libnet_ether_addr *)&empty_mac,
			sizeof(struct libnet_ether_addr));

	/* convert dest IP from string */
	if (inet_aton(dest_ip, &request.ar_tpa) == 0) {
		fprintf(stderr, "Invalid Destination IP Address\n");
		libnet_destroy(context);
		exit(1);
	}

	/* build arp header! */
	arp_tag = libnet_build_arp(ARPHRD_ETHER, ETHERTYPE_IP,
			MACADDR_LEN, IPADDR_LEN, 
			(reply ? ARPOP_REPLY : ARPOP_REQUEST),
			(u_char *)&request.ar_sha, (u_int8_t *)&request.ar_spa,
			(u_char *)&request.ar_tha, (u_int8_t *)&request.ar_tpa,
			payload, payload_len,
			context, arp_tag);

	/* build ethernet header */
	ether_tag = libnet_build_ethernet(broadcast_mac,
		(u_char *)&request.ar_sha, ETHERTYPE_ARP, NULL, 0,
		context, ether_tag);

	printf("Sending packet...\n");
	numbytes = libnet_write(context);
	if (numbytes == -1) {
		fprintf(stderr, "Error writing packet\n");
		libnet_destroy(context);
		exit(1);
	}

	printf("Done!\n");
	libnet_destroy(context);

	return 0;
}
