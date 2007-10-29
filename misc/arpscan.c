#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/bpf.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

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
	char * device = "en1";
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

struct arppl
{
	u_char sha[6];
	u_char spa[4];
	u_char tha[6];
	u_char tpa[4];	
};
#define IP_ADDR_LEN 4

int main(int argc, char * argv[])
{
	char errbuf[BUFSIZ];
	int fd, c;
	int i, j, k, l;
	struct ether_header * eth;
	struct arphdr * arp_header;
	struct arppl * payload;
	u_char * packet;
	int packet_len;
	u_char mask[IP_ADDR_LEN] = { 255, 255, 255, 0 };
	u_char net[IP_ADDR_LEN] = { 192, 168, 11, 0 };
	u_char sha[ETHER_ADDR_LEN] = { 0x00, 0x0d, 0x93, 0x82, 0x18, 0x1d };
	u_char bha[ETHER_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	u_char spa[IP_ADDR_LEN] = { 192, 168, 11, 102 };
	u_char tpa[IP_ADDR_LEN] = { 0, 0, 0, 0 };

	packet_len = sizeof(struct arppl) + sizeof(struct arphdr) +
		sizeof(struct ether_header);
	packet = malloc(packet_len);

	eth = (struct ether_header *)packet;
	arp_header = (struct arphdr *)(eth + 1);
	payload = (struct arppl *)(arp_header + 1);

	memcpy(payload->sha, sha, 6);
	memcpy(payload->spa, spa, 4);
	memcpy(payload->tha, bha, 6);
	memcpy(payload->tpa, tpa, 4);

	arp_header->ar_hrd = ARPHRD_ETHER;
	arp_header->ar_pro = ETHERTYPE_IP;
	arp_header->ar_hln = 6;
	arp_header->ar_pln = 4;
	arp_header->ar_op = ARPOP_REQUEST;

	memcpy(eth->ether_dhost, bha, 6);	
	memcpy(eth->ether_shost, sha, 6);
	eth->ether_type = ETHERTYPE_ARP;

	fd = bpf_open(errbuf);
	if (fd == -1)
	{
		fprintf(stderr, "%s", errbuf);
		exit(1);
	}
	bpf_init(fd);

	for (i = mask[0]; i <= 255; i++)
	{
		tpa[0] = net[0] + (255 - i);
		for (j = mask[1]; j <= 255; j++)
		{
			tpa[1] = net[1] + (255 - j);
			for (k = mask[2]; k <= 255; k++)
			{
				tpa[2] = net[2] + (255 - k);
				for (l = mask[3]; l <= 255; l++)
				{
					tpa[3] = net[3] + (255 - l);
	printf("Scanning %s\n",
		inet_ntoa(*(struct in_addr *)tpa));
					memcpy(payload->tpa, tpa, 4);

					c = write(fd, packet, packet_len);
					if (c != packet_len)
					{
	fprintf(stderr, "%s(): %d bytes written (%s)\n",
		__func__, c, strerror(errno));
					}
				}
			}
		}
	}
printf("Done!\n");

	return 0;
}
