/*
 * trace.c - Send a packet with custom IP options to try a trace
 * This code doesn't work yet.
 * Allen Porter <allen@thebends.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <libnet.h>
#include <libnet/libnet-headers.h>
#include <libnet/libnet-functions.h>

#define MACADDR_LEN 6
#define IPADDR_LEN 4

struct ip_options
{
	u_int8_t type;
	u_int8_t length;
	u_int8_t ptr;
	u_char data[17];
};

void usage(char *argv[]) {
	fprintf(stderr, "Usage: %s <ip>\n", argv[0]);
}

int main(int argc, char *argv[]) {
	char errbuf[LIBNET_ERRBUF_SIZE];
	libnet_ptag_t ip_tag = LIBNET_PTAG_INITIALIZER;
	libnet_ptag_t ipopt_tag = LIBNET_PTAG_INITIALIZER;
	libnet_ptag_t icmp_tag = LIBNET_PTAG_INITIALIZER; 
	libnet_t * context;
	int numbytes;				/* bytes written */
	char * dest_ip = NULL;
	u_int32_t dest, src;
	char * payload = "111";
	char * dev = "en1";
	int payload_len = strlen(payload);
	int packet_len;
	int id;
	int seq;
	struct ip_options options;

	if (argc < 2)
	{
		usage(argv);
		exit(1);
	}

	srand(time(NULL));
       	id = (int)rand();
	seq = (int)rand();
	dest_ip = strdup(argv[1]);

	// copied=0, number=0, type=7
	options.type = 7;
	options.length = 20;
	options.ptr = 3;
	memset(&options.data, 0, 17);

       	packet_len = payload_len + sizeof(struct icmp);
/*	
	printf("[%d]\n", packet_len);
	printf("[%d]\n", payload_len);
	printf("[%d]\n", (int)sizeof(struct icmp));
	printf("[%d]\n", (int)sizeof(struct ip));
*/
	context = libnet_init(LIBNET_RAW4, dev, errbuf);
	if (context == NULL) {
		fprintf(stderr, "Unable to initialize library: %s",
				errbuf);
		exit(1);
	}

	src = libnet_get_ipaddr4(context);
	/* convert dest IP from string */
	if (inet_aton(dest_ip, (struct in_addr *)&dest) == 0) {
		fprintf(stderr, "Invalid Destination IP Address\n");
		exit(1);
	}

	icmp_tag = libnet_build_icmpv4_echo( ICMP_ECHO,
			0, 0, id, seq, payload, payload_len,
			context, icmp_tag);

	ipopt_tag = libnet_build_ipv4_options((u_int8_t *)&options,
			options.length, context, ipopt_tag);

	ip_tag = libnet_build_ipv4(packet_len + 20,
			0, id, 0, 65, IPPROTO_ICMP,
			0, src, dest, NULL, 0, context, ip_tag);

	printf("Sending packet...\n");
	numbytes = libnet_write(context);
	if (numbytes == -1) {
		fprintf(stderr, "Error writing packet\n");
		exit(1);
	}

	printf("Done! [%d]\n", numbytes);
	libnet_destroy(context);

	return 0;
}
