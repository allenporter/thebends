/*
 * utunnel
 * Copyright (c) 2004 Allen Porter <allen@thebends.org>
 *
 * packet-funcions.c
 */
#include "packet-functions.h"

void dump_ip_packet(struct ip * packet)
{
        printf("tos: %x len: %x id: %x off: %x ttl %x\n",
                packet->ip_tos, packet->ip_len,
                packet->ip_id, packet->ip_off,
                packet->ip_ttl);
        printf("ip src: %s", inet_ntoa(packet->ip_src));
        printf("\tip dst: %s\n", inet_ntoa(packet->ip_dst));
}

void dump_udp_packet(struct udphdr * packet)
{
        printf("sport: %d dport: %d len: %d sum %d\n",
                packet->uh_sport, packet->uh_dport,
                packet->uh_ulen, packet->uh_sum);
}

/*
 * Name: ntoh_iphdr
 * Desc: Converts an IP packet header from Network Byte Order to Host Byte
 *       Order via calls to ntohl and ntohs on appropriate ip header fields.
 * Args: hdr => The header struct of an IP packet
 */
void
ntoh_iphdr(struct ip * hdr)
{
	hdr->ip_v = ntohl(hdr->ip_v);
	hdr->ip_hl = ntohl(hdr->ip_hl);
	hdr->ip_len = ntohs(hdr->ip_len);
	hdr->ip_id = ntohs(hdr->ip_id);
	hdr->ip_off = ntohs(hdr->ip_off);
	hdr->ip_sum = ntohs(hdr->ip_sum);
//	hdr->ip_src.s_addr = ntohl(hdr->ip_src.s_addr);
//	hdr->ip_dst.s_addr = ntohl(hdr->ip_dst.s_addr);
}

void
ntoh_udphdr(struct udphdr * hdr)
{
	hdr->uh_sport = ntohs(hdr->uh_sport);
	hdr->uh_dport = ntohs(hdr->uh_dport);
	hdr->uh_ulen = ntohs(hdr->uh_ulen);
	hdr->uh_sum = ntohs(hdr->uh_sum);
}

int
macaddr_set(struct ether_addr * mac, char * mac_address)
{
	struct ether_addr * addr;
	
	addr = (struct ether_addr *)ether_aton(mac_address);
	if (addr == NULL)
		return 0;	/* failure */

	memcpy(&mac, addr, sizeof(struct ether_addr));

	return 1;	/* success */
}

/*
 * create an endpoint record from an address and port combination
 */
void
endpoint_create(endpoint * point, struct in_addr addr, int port)
{
	memcpy(&point->addr, &addr, sizeof(struct in_addr));
	point->port = port;
}


/*
 * writes the specified ip packet on the eternal network device
 */
void
write_external(uconfig * config, endpoint * dest_endp,
		u_char * packet, u_int packet_len)
{
	char errbuf[LIBNET_ERRBUF_SIZE];	/* libnet error buffer */
	libnet_t * context;			/* libnet context struct */
	libnet_ptag_t ip_tag = LIBNET_PTAG_INITIALIZER,	/* ip packet tag */
		udp_tag = LIBNET_PTAG_INITIALIZER;	/* udp packet tag */

	srand(time(NULL));

	printf("ext [%s]", inet_ntoa(config->source.addr)),
	printf("==>[%s]\n", inet_ntoa(dest_endp->addr));

	/* TODO - re-implement mac address spoofing */

	/* initialize libnet on external device */
	context = libnet_init(LIBNET_RAW4, config->external_dev, errbuf);
	if (context == NULL)
	{
		fprintf(stderr, "Unable to initialize libnet: %s\n",
				errbuf);
		exit(1);
	}

	/* TODO - allow for other protocol types besides UDP ? */
	udp_tag = libnet_build_udp(
			config->source.port, dest_endp->port,
			LIBNET_UDP_H + packet_len,
			LIBNET_AUTO_CHECKSUM,
			packet, packet_len,
			context, 0);
	if (udp_tag == -1)
	{
		fprintf(stderr, "Error building UDP packet: %s\n",
			libnet_geterror(context));
		exit(1);
	}

	/* initialize ip packet header */
	/* TODO - should we do something less predictable here? */

	ip_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + LIBNET_UDP_H + packet_len,
			0, rand(), 0, 64, IPPROTO_UDP,
			LIBNET_AUTO_CHECKSUM,
			config->source.addr.s_addr,
			dest_endp->addr.s_addr,
			NULL, 0,
			context, 0);
	if (ip_tag == -1)
	{
		fprintf(stderr, "Error building IP packet: %s\n",
			libnet_geterror(context));
		exit(1);
	}

	if (libnet_write(context) == -1)
	{
		fprintf(stderr, "Error writing packet: %s\n",
			libnet_geterror(context));
		exit(1);
	}

	/* clean up!*/
	libnet_destroy(context);
}

/*
 * writes the specified ip packet on the loopback device
 */
void
write_loopback(uconfig * config, endpoint * source_endp,
		u_char * packet, u_int packet_len)
{
	char errbuf[LIBNET_ERRBUF_SIZE];	/* libnet error buffer */
	libnet_t * context;			/* libnet context struct */
	libnet_ptag_t ip_tag = LIBNET_PTAG_INITIALIZER;	/* ip packet tag */
	u_char * payload;
	u_int payload_len;
	struct ip * iphdr;

	iphdr = (struct ip *)packet;

printf("---------------------------------------------------\n");
dump_ip_packet(iphdr);

        /* transform the IP packet using the session information */
        memcpy(&iphdr->ip_src, &source_endp->addr,
			sizeof(struct in_addr));
        inet_aton(LOOPBACK_IP, &iphdr->ip_dst);

dump_ip_packet(iphdr);
printf("---------------------------------------------------\n");

	/* separate IP payload */	
	payload = (u_char *)(iphdr + 1);
	payload_len = packet_len - sizeof(struct ip);

	/* initialize libnet on loopback device */
	context = libnet_init(LIBNET_RAW4, LOOPBACK_DEV, errbuf);
	if (context == NULL)
	{
		fprintf(stderr, "write_loopback(): %s", errbuf);
		exit(1);
	}

	/* TODO - verbosity */
	printf("loop [%s]", inet_ntoa(iphdr->ip_src));
	printf("==>[%s]\n", inet_ntoa(iphdr->ip_dst));

	/* initialize an ip packet from the loopback tunnel to localhost */
	ip_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + payload_len,
			iphdr->ip_tos, iphdr->ip_id,
			iphdr->ip_off, iphdr->ip_ttl,
			iphdr->ip_p,
			LIBNET_AUTO_CHECKSUM,
			iphdr->ip_src.s_addr, iphdr->ip_dst.s_addr,
			payload, payload_len,
			context, ip_tag);
	if (!ip_tag)
	{
		fprintf(stderr, "Error building IP packet: %s\n",
				libnet_geterror(context));
		exit(1);
	}	

	if (libnet_write(context) == -1)
	{
		fprintf(stderr, "Error writing packet: %s\n",
				libnet_geterror(context));
		exit(1);
	}

	/* clean up!*/
	libnet_destroy(context);
}

/*
 * initialize the packet capture and filter routines
 */
void
sniffer_init(uconfig * config, char * device, char * filter_string,
		pcap_handler callback)
{
	char errbuf[PCAP_ERRBUF_SIZE];	/* pcap error buffer */
	pcap_t * pcap_handle;		/* for pcap state */
	struct bpf_program bpf;		/* packet filter info */
	bpf_u_int32 net;		/* network of sniff device */
	bpf_u_int32 mask;		/* mask of sniff device */
        int immediate=1;		/* BIOCIMMEDIATE */

	/* initialize the filtering structure */
	memset(&bpf, 0, sizeof(struct bpf_program));

	printf("Openinig %s [%s]\n", device, filter_string);

	/* open a handle on the network device to sniff */
	pcap_handle = pcap_open_live(device, SNAPLEN, 1, READ_TIMEOUT, errbuf);
	if (!pcap_handle)
	{
		fprintf(stderr, "sniffer_init(): %s\n", errbuf);
		exit(1);
	}

#ifdef BIOCIMMEDIATE
	/* thank you Dan Kaminsky! makes pcap unbuffered */
        ioctl(pcap_fileno(pcap_handle), BIOCIMMEDIATE, &immediate);
#endif

	/* get network and mask for compiling filter string */	
	if (pcap_lookupnet(device, &net, &mask, errbuf) == -1)
	{
		fprintf(stderr, "%s\n", errbuf);
		exit(1);
	}

	/* compile the filter string and apply the filter to the handle */
	pcap_compile(pcap_handle, &bpf, filter_string, 0, net);
	pcap_setfilter(pcap_handle, &bpf);

	/* enter the packet capture loop and block until exit */
	pcap_loop(pcap_handle, 0, callback, (u_char *)config);

	/* TODO -- will this ever happen? maybe handle signals better? */
	
	/* clean up! */
	pcap_close(pcap_handle);
}
