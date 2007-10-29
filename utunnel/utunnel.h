/*
 * udptunnel
 * Copyright (c) 2004 Allen Porter <allen@thebends.org>
 *
 * udptunnel.h
 */

#ifndef __UTUNNEL_H__
#define __UTUNNEL_H__

#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <net/bpf.h>
#include <libnet.h>
#include <libnet/libnet-macros.h>
#include <libnet/libnet-structures.h>
#include <libnet/libnet-headers.h>
#include <libnet/libnet-functions.h>
#include <pcap.h>
#include "config.h"

#define LIBNET_AUTO_CHECKSUM 0		/* auto compute our checksum */
#define SNAPLEN 65535
#define READ_TIMEOUT 500
#define DEVICE_LEN 6
#define FILTER_LEN 256
#define IPADDR_LEN 16
#define LOOPBACK_IP "127.0.0.1"
#define MAX_CLIENTS 10
#define LOOPBACK_DEV "lo0"

/* a null mac address */
extern u_int8_t empty_mac[6];

typedef enum daemon_info
{
	SERVER = 1,
	CLIENT
} daemon_type;

/* tunnel endpoint */
typedef struct session_endpoint
{
	struct in_addr addr;
	int port;
} endpoint;

/* configuration for the udp tunnel */
typedef struct utunnel_config
{
	endpoint source;	/* ip/port for external packet */
	endpoint dest;		/* ignored in server config */
	struct ether_addr mac;	/* mac address of external packets */
	char external_dev[DEVICE_LEN];		/* sniff device */
	daemon_type type;	/* client or server? */
	pcap_handler ext_handler;
	pcap_handler loop_handler;
} uconfig;

struct translation_info
{
	endpoint source;
	u_char * payload;
	int payload_len;
};

void init_config(uconfig * config);
void loopback_init(uconfig * config);
void external_init(uconfig * config);
void init_threads(uconfig * config);

#endif /* __UTUNNEl_H__ */
