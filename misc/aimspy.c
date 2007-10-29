/*
 * aimspy.c
 * 2004 Alen Porter <allen@thebends.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <pcap.h>	
#include <net/if_arp.h>

#define SNAPLEN 65535           /* maximum number of bytes captured */
#define READ_TIMEOUT 1000

struct flap_header
{
	u_int8_t id;
	u_int8_t channel;
	u_int8_t seq_number[4];
	u_int16_t family;
	u_int16_t subtype;
};

struct aim_message
{
	u_int16_t flags;
	u_int8_t request[4];
	u_int8_t cookie[8];
	u_int16_t channel_id;
	u_int8_t screenname_len;
	// variable length screenname
	// TLVs
};

struct tlv
{
	u_int16_t type;
	u_int16_t data_len;
};

/*
 * callback function, run when a packet arrives
 */
void got_packet(u_char *args, struct pcap_pkthdr *header, u_char *packet)
{
	struct ether_header * ethr_hdr;
	struct ip * ip_hdr;
	struct tcphdr * tcp_hdr;
	struct flap_header * flap_hdr;
	struct aim_message * aim_hdr;
	struct tlv * tlv_hdr;
	u_char * payload;
	int payload_len;
	char screenname[255];
	char message[4096];
	
	/* split the packet into ethernet header, arp header, and
	 * arp payload that contains MAC and IP addresses of ARP 
	 * request
	 */
	ethr_hdr = (struct ether_header *)packet;
	ip_hdr = (struct ip *)(ethr_hdr + 1);
	tcp_hdr = (struct tcphdr *)(ip_hdr + 1);
	payload_len = ip_hdr->ip_len - sizeof(struct ip) -
		sizeof(struct tcphdr);
	
	// nothing to do hear?	
	if (payload_len < sizeof(struct flap_header))
		return;

	flap_hdr = (struct flap_header *)(tcp_hdr + 1);

	// only handle "messaging" family
	if (flap_hdr->family != 4)
		return;

	if (flap_hdr->subtype != 6 && flap_hdr->subtype != 7)
		return;

	aim_hdr = (struct aim_message *)(flap_hdr + 1);
		
	// ignore
	if (aim_hdr->channel_id != 1)
		return;

	if (aim_hdr->screenname_len >= 255)
		return;

	payload = (u_char *)(aim_hdr + 1);
	payload--;

	strncpy(&screenname[0], payload, aim_hdr->screenname_len);
	screenname[aim_hdr->screenname_len] = NULL;
	
	if (flap_hdr->subtype == 6)
	{
		// sent message
		tlv_hdr = (struct tlv *)(payload + aim_hdr->screenname_len); 

	}
	else
	{
		// received message
		payload = (u_char *)(payload + aim_hdr->screenname_len); 
		
		// skip warning level & number of TLVs
		payload += 2;

//		printf("TLVs: [%x][%x]\n", payload[0], payload[1]);

		payload += 2;

		tlv_hdr = (struct tlv *)(payload);
		while (tlv_hdr != NULL && tlv_hdr->type != 2)
		{
			/*
			printf("TLV Type %d, Skipping %d bytes\n",
					tlv_hdr->type,
					tlv_hdr->data_len);
					*/
			payload = (u_char *)(tlv_hdr + 1) + tlv_hdr->data_len;
			tlv_hdr = (struct tlv *)payload;
		}
	}

//	printf("[%d][%d]\n", tlv_hdr->type, tlv_hdr->data_len);

	payload = (u_char *)(tlv_hdr + 1);
	payload_len = *(u_int16_t *)(payload + 2);

	payload = (u_char *)(payload + 4 + payload_len);
	payload_len = *(u_int16_t *)(payload + 2) - 4;

	payload += 4;

	strncpy(message, payload + 4, payload_len);
	message[payload_len] = NULL;

	printf("%s %s: %s\n", (flap_hdr->subtype == 6) ? ">" : "<",
			screenname, message);

	printf("\n");
}

int main(int argc, char *argv[])
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;			/* session handler */
	struct bpf_program bpf;		/* stores the filter */
	char filter[] = "port 5190";	/* text version of filter */
	bpf_u_int32 mask;
	bpf_u_int32 net;
	char *net_s;
	char *mask_s;
	int ret;
	struct in_addr addr;
	char *dev = argv[1];

	if (argc != 2)
	{
		printf("Usage: aimspy <device>\n");
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

	printf("Sniffing AIM traffic...\n\n");

	/* capture packets forever! */
	pcap_loop(handle, 0, (pcap_handler)got_packet, NULL);

	/* close session */
	pcap_close(handle);

	return 0;
}
