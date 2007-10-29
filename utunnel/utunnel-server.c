/* 
 * utunnel
 * Copyright (c) 2004 Allen Porter <allen@thebends.org>
 *
 * utunnel-server.c
 */

#include "utunnel.h"
#include "translate.h"
#include "packet-functions.h"

extern char *optarg;

void
usage()
{
	printf("usage: utunnel-server -i <device> -p <port> -s <ip addr>\n",
		" [-m <mac>]\n");
	exit(0);
}

void
handle_arguments(int argc, char * argv[], uconfig * config)
{
	char ch;
	int arg_sport = 0, arg_dport = 0;

	while ((ch = getopt(argc, argv, "i:p:s:m:")) != EOF)
	{
		switch (ch)
		{
			case 'p':
				arg_sport++;
				config->source.port = atoi(optarg);
				break;
			case 's':
				if (inet_aton(optarg, &config->source.addr)==0)
				{
					fprintf(stderr, "Bad IP Address: %s",
							optarg);
					exit(1);
				}
				break;
			case 'm':
				if (macaddr_set(&config->mac, optarg)==0)
				{
					fprintf(stderr, "Bad Source Mac: %s\n",
							optarg);
					exit(1);
				}
				break;
			case 'i':
				strncpy(config->external_dev, optarg,
					DEVICE_LEN);
				break;
			case '?':
			default:
				usage();
				break;
		}
	}

	if (strlen(config->external_dev) == 0 ||
			config->source.addr.s_addr == 0 ||
			arg_sport == 0)
		usage();
}

/*
 * callback function, run when a udp packet is received on the external
 * interface directed at the configured IP and port.
 */
void
external_packet(u_char *args, struct pcap_pkthdr *header, u_char *packet)
{
	struct ether_header * ether_header;
	struct ip * ip_header;
	struct udphdr * udp_header;

	struct ip * ip_hdr;
	u_char * payload;			/* udp packet payload */
	int payload_len;
	endpoint client_endpoint;		/* client ip/port combo */
	session * client_session;		/* session for this packet */
	uconfig * config;			/* configuration */

	
	config = (uconfig *)args;

	// TODO - add some verbosity levels
	printf("Incoming packet on %s\n", config->external_dev);

	ether_header = (struct ether_header *)packet;
	ip_header = (struct ip *)(ether_header + 1);
	udp_header = (struct udphdr *)(ip_header + 1);

	ntoh_iphdr(ip_header);
	ntoh_udphdr(udp_header);

//dump_ip_packet(ip_header);
//dump_udp_packet(udp_header);

	endpoint_create(&client_endpoint, ip_header->ip_src,
			udp_header->uh_sport);

	/* lookup existing session or create new using client endpoint */
	client_session = session_lookup(&client_endpoint, INCOMING);
	if (client_session == NULL)
		client_session = session_create(&client_endpoint);

	payload = (u_char *)(udp_header + 1);
	payload_len = header->len - (payload - packet);

	/* ip header is at start of udp payload data, convert to host
	 * byte order
	 */
	ip_hdr = (struct ip *)payload;

	ntoh_iphdr(ip_hdr);

	printf("Write LB: [%s]\n", inet_ntoa(client_session->internal.addr));

	/* dump it on the loopback wire */
	write_loopback(config, &client_session->internal, payload, payload_len);
}

void
loopback_packet(u_char *args, struct pcap_pkthdr *header, u_char *packet)
{
	session * client_session;		/* session for this packet */
	endpoint client_endpoint;
	struct ip * ip_hdr;
	u_char * payload;
	u_int payload_len;
	uconfig * config;		/* configuration */

	config = (uconfig *)args;

printf("Incoming packet on loopback\n", config->external_dev);

	/* skip 4 bytes of loopback header */
	ip_hdr = (struct ip *)(packet + 4); 

	memcpy(&client_endpoint.addr, &ip_hdr->ip_dst, sizeof(struct in_addr));

	/* lookup existing session or create new using client endpoint */
	client_session = session_lookup(&client_endpoint, OUTGOING);
	if (client_session == NULL)
	{
		fprintf(stderr, "loopback_packet(): Ignoring packet "
			       "with abandoned session\n");
		return;
	}
		
	payload = (u_char *)ip_hdr;
	payload_len = header->len - 4;

printf("session: [%s:%d] ", inet_ntoa(client_session->external.addr),
			client_session->external.port);
printf("[%s:%d]\n", inet_ntoa(client_session->internal.addr),
			client_session->internal.port);

	/* encapsulate layer 2 and up in udp */
	write_external(config, &client_session->external, payload, payload_len);
}

int
main(int argc, char * argv[])
{
	uconfig config;

	init_config(&config);
	config.type = SERVER;

	/* set callback functions for packet handling */
	config.ext_handler = (pcap_handler)external_packet;
	config.loop_handler = (pcap_handler)loopback_packet;

	handle_arguments(argc, argv, &config);

	init_threads(&config);

	return 0;
}
