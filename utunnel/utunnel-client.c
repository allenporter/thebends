/*
 * utunnel
 * Copyright (c) 2004 Allen Porter <allen@thebends.org>
 *
 * utunnel-client.c
 */

#include "config.h"
#include "utunnel.h"
#include "packet-functions.h"

extern char *optarg;

void
usage()
{
	printf("usage: utunnel-client <options>\n"
		"\n"
		"UDP Options:    * = Optional Argument\n"
		"   -p <local port>\n"
		"   -r <remote port>\n\n"
		"IP Options:\n"
		"   -s <source ip address>\n"
		"   -d <dest ip address>\n"
		"Data Link Options:\n"
		" * -i <ethernet device>\n"
		" * -m <source mac address>\n");

	exit(0);
}

void
handle_arguments(int argc, char * argv[], uconfig * config)
{
	char ch;
	int arg_sport = 0, arg_dport = 0;

	while ((ch = getopt(argc, argv, "p:r:s:d:i:m:")) != EOF)
	{
		switch (ch)
		{
			case 'p':
				arg_sport++;
				config->source.port = atoi(optarg);
				break;
			case 'r':
				arg_dport++;
				config->dest.port = atoi(optarg);
				break;
			case 's':
				if (inet_aton(optarg, &config->source.addr)==0)
				{
					fprintf(stderr, "Bad Source IP: %s",
							optarg);
					exit(1);
				}
				break;
			case 'd':
				if (inet_aton(optarg, &config->dest.addr)==0)
				{
					fprintf(stderr, "Bad Dest IP: %s\n",
							optarg);
					exit(1);
				}
				break;
			case 'm':
				if (!macaddr_set(&config->mac, optarg))
				{
					fprintf(stderr, "Bad Mac: %s\n",
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

	if (config->source.addr.s_addr == 0 ||
		config->dest.addr.s_addr == 0 ||
		arg_sport == 0 || arg_dport == 0)
	{	
		usage();
	}
}

void
external_packet(u_char *args, struct pcap_pkthdr *header, u_char *packet)
{
	u_char tunnelip[4] = { 127, 0, 0, 2 };

	struct ether_header * ether_header;
	struct ip * ip_header;
	struct udphdr * udp_header;

	struct ip * ip_hdr;
	u_char * payload;
	int payload_len;
	endpoint src;
	uconfig * config;

	config = (uconfig *)args;

	printf("Incoming packet on %s\n", config->external_dev);

	ether_header = (struct ether_header *)packet;
	ip_header = (struct ip *)(ether_header + 1);
	udp_header = (struct udphdr *)(ip_header + 1);

	payload = (u_char *)(udp_header + 1);
	payload_len = header->len - (payload - packet);

	ip_hdr = (struct ip *)payload;
	
	/* convert ip header to host byte order */
	ntoh_iphdr(ip_hdr);

	/* TODO - this is pretty ugly, isnt it? */
	memcpy(&src.addr, tunnelip, sizeof(struct in_addr));
 
	/* write raw ip payload to loopback */	
	write_loopback(config, &src, payload, payload_len);	
}

void
loopback_packet(u_char *args, struct pcap_pkthdr *header, u_char *packet)
{
	struct ip * ip_hdr;
	u_char *payload;
	u_int payload_len;
	uconfig * config;

	config = (uconfig *)args;

	/* skip 4 bytes of loopback header */
	ip_hdr = (struct ip *)(packet + 4); // + sizeof(struct ether_header));

	payload = (u_char *)ip_hdr;
	payload_len = header->len - 4; //sizeof(struct ether_header);

	printf("[%s]->", inet_ntoa(ip_hdr->ip_src));
	printf("[%s]\n", inet_ntoa(ip_hdr->ip_dst));
	printf("Received loopback packet, payload size %d\n", payload_len);

	/* encapsulate layer 2 and up in udp */
	write_external(config, &config->dest, payload, payload_len);
}

int
main(int argc, char * argv[])
{
	uconfig config;

	init_config(&config);
	config.type = CLIENT;
	config.ext_handler = (pcap_handler)external_packet;
	config.loop_handler = (pcap_handler)loopback_packet;

	handle_arguments(argc, argv, &config);

	init_threads(&config);

	return 0;
}
