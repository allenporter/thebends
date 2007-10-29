/*
 * arpspy.
 * Copyright (c) 2004 Allen Porter <allen@thebends.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <pcap.h>	
#include <net/if_arp.h>

#define SNAPLEN 65535           /* maximum number of bytes captured */
#define READ_TIMEOUT 500

#define ETHER_NTOA(a) (ether_ntoa((struct ether_addr *)&(a)))
#define INET_NTOA(a) (inet_ntoa(*(struct in_addr *)(&a)))

/* payload contained inside an arp packet */
struct arp_payload {
	u_char ar_sha[6];
	u_char ar_spa[4];
	u_char ar_tha[6];
	u_char ar_tpa[4];
};

/*
 * packet_ntohs - converts from network to host byte order for essential
 * fields.
 */
void packet_ntohs(struct ether_header *ethr_hdr, struct arphdr *arp_hdr)
{
	ethr_hdr->ether_type = ntohs(ethr_hdr->ether_type);
        arp_hdr->ar_hrd = ntohs(arp_hdr->ar_hrd);
        arp_hdr->ar_pro = ntohs(arp_hdr->ar_pro);
        arp_hdr->ar_op = ntohs(arp_hdr->ar_op);
	return;
}

/*
 * validate_headers - make sure this is a packet we know how to handle.
 */
void validate_headers(const struct ether_header * ethr_hdr,
		const struct arphdr * arp_hdr)
{
	if (ethr_hdr->ether_type != ETHERTYPE_ARP)
	{
		fprintf(stderr, "Not an ARP packet! [Ether Type = 0x%x]\n",
				ethr_hdr->ether_type);
		exit(1);
	}
	if (arp_hdr->ar_hrd != ARPHRD_ETHER)
	{
		fprintf(stderr, "Not an Ethernet Packet! [Hardware = 0x%x]\n",
				arp_hdr->ar_hrd);
		exit(1);
	}
	if (arp_hdr->ar_pro != ETHERTYPE_IP)
	{
		fprintf(stderr, "Datagram type not IP [0x%x]\n",
				arp_hdr->ar_pro);
		exit(1);
	}
	if (arp_hdr->ar_hln != sizeof(struct ether_addr))
	{
		fprintf(stderr, "Unsupported Hardware Addr Len [0x%x]\n", 
				arp_hdr->ar_hln);
		exit(1);
	}
	if (arp_hdr->ar_pln != sizeof(struct in_addr))
	{
		fprintf(stderr, "Unsupported Protocol Addr Len [0x%x]\n",
				arp_hdr->ar_pln);
		exit(1);
	}
	if (arp_hdr->ar_op != ARPOP_REQUEST &&
			arp_hdr->ar_op != ARPOP_REPLY)
	{
		fprintf(stderr, "Unsupported operation [0x%x]\n",
				arp_hdr->ar_op);
	}

	/* packet looks ok to me! */
	return;
}

/*
 * print the contents of an arp packet
 */
void print_arp(const struct arphdr *arp_hdr,
		const struct arp_payload *payload)
{
	if (arp_hdr->ar_op == ARPOP_REQUEST)
	{
		printf("Request from %s ", INET_NTOA(payload->ar_spa));
		printf("[%s] ", ETHER_NTOA(payload->ar_sha));
		printf("for %s ", INET_NTOA(payload->ar_tpa));
		printf("[%s]\n", ETHER_NTOA(payload->ar_tha));
	}
	else if (arp_hdr->ar_op == ARPOP_REPLY)
	{
		printf("Reply from %s ", INET_NTOA(payload->ar_spa));
		printf("[%s] ", ETHER_NTOA(payload->ar_sha));
		printf("to %s ", INET_NTOA(payload->ar_tpa));
		printf("[%s]\n", ETHER_NTOA(payload->ar_tha));
	}
	else
	{
		fprintf(stderr, "Unknown ARP operation");
		exit(1);
	}

	printf("\n");

	return;
}

/*
 * callback function, run when a packet arrives
 */
void got_packet(u_char *args, struct pcap_pkthdr *header,
	u_char *packet)
{
	struct ether_header * ethr_hdr;
	struct arphdr * arp_hdr;
	struct arp_payload * payload;
	
	/* split the packet into ethernet header, arp header, and
	 * arp payload that contains MAC and IP addresses of ARP 
	 * request
	 */
	ethr_hdr = (struct ether_header *)packet;
	arp_hdr = (struct arphdr *)(ethr_hdr + 1);
	payload = (struct arp_payload *)(arp_hdr + 1);

	/* convert packet to host byte order and validate that we can
	 * handle the packet
	 */
	packet_ntohs(ethr_hdr, arp_hdr);
	validate_headers(ethr_hdr, arp_hdr);

	printf("Ethernet Dest: %s ", ETHER_NTOA(ethr_hdr->ether_dhost));
	printf("Source: %s\n", ETHER_NTOA(ethr_hdr->ether_shost));

	print_arp(arp_hdr, payload);		
}

int main(int argc, char *argv[])
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;				/* session handler */
	struct bpf_program bpf;		/* stores the filter */
	char filter[] = "arp";			/* text version of filter */
	bpf_u_int32 mask;
	bpf_u_int32 net;
	char *net_s;
	char *mask_s;
	int ret;
	struct in_addr addr;
	char *dev = argv[1];

	if (argc != 2)
	{
		printf("Usage: arpspy <device>\n");
		exit(0);
	}
	if (getuid() != 0)
	{
		fprintf(stderr, "You must be root to use this program.\n");
		exit(1);
	}

	// write zeros into the struct.. why?
	memset(&bpf, 0, sizeof(struct bpf_program));

	printf("DEV: %s\n", dev);
	/* sniff promiscuosly until an error occurs */
	handle = pcap_open_live(dev, SNAPLEN, 1, READ_TIMEOUT, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "%s\n", errbuf);
		exit(1);
	}

	/* return the IP and netmask of the device */
	ret = pcap_lookupnet(dev, &net, &mask, errbuf);
	if (ret == -1)
	{
		fprintf(stderr, "%s\n", errbuf);
		exit(1);
	}
	
	addr.s_addr = net;
	net_s = inet_ntoa(addr);
	if (net == NULL)
	{
		fprintf(stderr, "inet_ntoa");
		exit(1);
	}
	printf("NET: %s\n", net_s);

	addr.s_addr = mask;
	mask_s = inet_ntoa(addr);
	if (mask_s == NULL)
	{
		fprintf(stderr, "inet_ntoa");
		exit(1);
	}
	printf("MASK: %s\n", mask_s);

	/* complie and set the filter */
	pcap_compile(handle, &bpf, filter, 0, net);
	pcap_setfilter(handle, &bpf);

	printf("Sniffing ARP traffic...\n\n");
	/* capture packets forever! */
	pcap_loop(handle, 0, (pcap_handler)got_packet, NULL);

	/* TODO - handle signals so this code is actually called */
	printf("Done!\n");

	/* close session */
	pcap_close(handle);

	return 0;
}
