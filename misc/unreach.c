/*
 * unreach.c - Sends an ICMP unreach packets to all new connections
 * Not sure if this code works, haven't tested it in awhile.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <pcap.h>

#define SNAPLEN 65535           /* maximum number of bytes captured */
#define READ_TIMEOUT 500

struct icmp_unreach
{
	u_int8_t type;
	u_int8_t code;
	u_int16_t cksum;
	u_int32_t unused;
	u_int8_t header[20];
	u_int8_t datagram[64];
};

int fd;	/* bpf descriptor */
char * device;

int
bpf_open(char *err_buf)
{
        int i, fd;
        char device[sizeof "/dev/bpf000"];

        for (i = 0;;i++)
        {
                sprintf(device, "/dev/bpf%d", i);

                fd = open(device, O_RDWR);
                if (fd == -1 && errno == EBUSY)
                        continue;
               else
                        break;
        }

        if (fd == -1)
                snprintf(err_buf, BUFSIZ, "%s(): open(): (%s): %s\n",
                        __func__, device, strerror(errno));

        return fd;
}

void
bpf_init(int fd)
{
        struct ifreq ifr;

        strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name) - 1);

        ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
        if (ioctl(fd, BIOCSETIF, (caddr_t)&ifr) == -1)
        {
                fprintf(stderr, "%s(): BIOCSETIF: (%s): %s\n",
                               __func__, device, strerror(errno));
                exit(1);
        }

        if (ioctl(fd, BIOCGDLT, (caddr_t)&ifr) == -1)
        {
                fprintf(stderr, "%s(): BIOCGDLT:  %s\n",
                                __func__, strerror(errno));
                exit(1);
        }
}

/*
 * From ping.c (from original nuke.c)
 */
unsigned short in_cksum(addr, len)
    u_short *addr;
    int len;
{
    register int nleft = len;
    register u_short *w = addr;
    register int sum = 0;
    u_short answer = 0;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(u_char *)(&answer) = *(u_char *)w ;
        sum += answer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
//    printf("Checksum: [%x|%d]\n", answer, len);
    return(answer);
}

void
send_unreach(struct ether_header *ethr_hdr, struct ip *ip_hdr,
		struct tcphdr *tcp_hdr)
{
	struct ether_header *ethr_out;
	struct ip *ip_out;
	struct icmp_unreach *icmp_out;
	u_char *packet;
	int packet_len;
	int c;

	packet_len = sizeof(struct ether_header) + sizeof(struct ip) +
		sizeof(struct icmp_unreach);
	packet = malloc(packet_len);
	bzero(packet, packet_len);

	ethr_out = (struct ether_header *)packet;
	ip_out = (struct ip *)(ethr_out + 1);
	icmp_out = (struct icmp_unreach *)(ip_out + 1);

	memcpy(ethr_out->ether_dhost, ethr_hdr->ether_shost, 6);
	memcpy(ethr_out->ether_shost, ethr_hdr->ether_dhost, 6);
	ethr_out->ether_type = ETHERTYPE_IP;

	ip_out->ip_v = 4;
	ip_out->ip_hl = 5; //sizeof(struct ip) / 4;
	ip_out->ip_tos = 0;
	ip_out->ip_len = sizeof(struct ip) + sizeof(struct icmp_unreach);
	ip_out->ip_id = rand();
	ip_out->ip_off = 0;
	ip_out->ip_ttl = 128;
	ip_out->ip_p = IPPROTO_ICMP;
	ip_out->ip_sum = 0;
	memcpy(&ip_out->ip_src.s_addr, &ip_hdr->ip_dst.s_addr, 4);
	memcpy(&ip_out->ip_dst.s_addr, &ip_hdr->ip_src.s_addr, 4);
	ip_out->ip_sum = in_cksum(ip_out, sizeof(struct ip));

	icmp_out->type = ICMP_UNREACH;
	icmp_out->code = ICMP_UNREACH_PORT;
	icmp_out->cksum = 0;
	memcpy(icmp_out->header, ip_hdr, 20);
	memcpy(icmp_out->datagram, ip_hdr + 1, 64);
	icmp_out->cksum = in_cksum(ip_out, sizeof(struct icmp_unreach));

	c = write(fd, packet, packet_len);
	if (c != packet_len)
	{
	        fprintf(stderr, "%s(): %d bytes written (%s)\n",
	                __func__, c, strerror(errno));
	}

	printf("Done!\n");
	return;
}

void
got_packet(u_char *args, struct pcap_pkthdr *header, u_char *packet)
{
	struct ether_header *ethr_hdr;
	struct ip *ip_hdr;
	struct tcphdr *tcp_hdr;
	char * src_mac, * dst_mac;
	char * src, * dst;

	ethr_hdr = (struct ether_header *)packet;
	ip_hdr = (struct ip *)(ethr_hdr + 1);
	tcp_hdr = (struct tcphdr *)(ip_hdr + 1);

	src_mac = strdup(ether_ntoa((struct ether_addr *)&ethr_hdr->ether_shost));
	dst_mac = strdup(ether_ntoa((struct ether_addr *)&ethr_hdr->ether_dhost));

	src = strdup(inet_ntoa(ip_hdr->ip_src));
	dst = strdup(inet_ntoa(ip_hdr->ip_dst));


	printf("%s => %s\n", src_mac, dst_mac);	
	printf("TCP connection started: %s => %s:%d\n",
			src, dst, tcp_hdr->th_dport);
	printf("\n");

	send_unreach(ethr_hdr, ip_hdr, tcp_hdr);

	free(src);
	free(dst);
	free(src_mac);
	free(dst_mac);
}

int main(int argc, char * argv[])
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;
	struct bpf_program bpf;
	char filter[] = "(tcp[tcpflags] & (tcp-syn) != 0) and " \
			"(tcp[tcpflags] & (tcp-ack) == 0)";
	bpf_u_int32 mask;
	bpf_u_int32 net;
	int ret;
	int immediate=1;                /* BIOCIMMEDIATE */
	device = argv[1];

	if (argc != 2)
	{
		fprintf(stderr, "Usage %s <device>\n", argv[0]);
		exit(0);
	}

	if (getuid() != 0)
	{
		fprintf(stderr, "You must be root to use this program.\n");
		exit(1);	
	}

	srand(time(NULL));
	
	memset(&bpf, 0, sizeof(struct bpf_program));
	
	handle = pcap_open_live(device, SNAPLEN, 1, READ_TIMEOUT, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "%s\n", errbuf);
		exit(1);
	}

#ifdef BIOCIMMEDIATE
	ioctl(pcap_fileno(handle), BIOCIMMEDIATE, &immediate);
#endif

	ret = pcap_lookupnet(device, &net, &mask, errbuf);
	if (ret == -1)
	{
		fprintf(stderr, "%s\n", errbuf);
		exit(1);
	}

	pcap_compile(handle, &bpf, filter, 0, net);
	pcap_setfilter(handle, &bpf);

	fd = bpf_open(errbuf);
	if (fd == -1)
	{
		fprintf(stderr, "%s", errbuf);
		exit(1);
	}
	bpf_init(fd);

	pcap_loop(handle, 0, (pcap_handler)got_packet, NULL);

	pcap_close(handle);

	return 0;
}
