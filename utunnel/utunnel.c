/*
 * utunnel
 * Copyright (c) 2004 Allen Porter <allen@thebends.org>
 *
 * udpclient.c
 */

#include "config.h"
#include "utunnel.h"
#include "packet-functions.h"
#include "translate.h"

u_int8_t empty_mac[6] = { 0, 0, 0, 0, 0, 0 };

/*
 * initialize the configuration struct
 */
void
init_config(uconfig * config)
{
	size_t needed;
	int mib[6];

	memset(&config->source, 0, sizeof(endpoint));
	memset(&config->dest, 0, sizeof(endpoint));
	strncpy(config->external_dev, "any", DEVICE_LEN);
	memset(&config->mac, 0, sizeof(struct ether_addr));
	session_init_table();
}

/*
 * initalize the loopback interface and tie the specified callback handler
 */
void
external_init(uconfig * config)
{
	char filter[FILTER_LEN];

	/* handle external packets directed to the listening endpoint */
	if (config->type == SERVER)
	{
		snprintf(filter, FILTER_LEN,
			"dst host %s and dst port %d and udp",
			inet_ntoa(config->source.addr), config->source.port);
	}	
	else
	{
		snprintf(filter, FILTER_LEN,
			"src host %s and src port %d and "
			"dst host %s and dst port %d and udp",
			strdup(inet_ntoa(config->dest.addr)), config->dest.port,
			strdup(inet_ntoa(config->source.addr)), config->source.port);
	}

	/* start listening */
	sniffer_init(config, config->external_dev, filter,
		config->ext_handler);
}

/*
 * initialize the loopback interface and tie the specified callback handler
 */
void
loopback_init(uconfig * config)
{
	char filter[FILTER_LEN];
	char device[DEVICE_LEN];
       
	/* handle all loopback ip traffic, let our callback handle the rest */
	if (config->type == SERVER)
	{
		snprintf(filter, FILTER_LEN, "src host 127.0.0.1 "
			"and not dst host 127.0.0.1 and ip");
	}
	else
	{
		snprintf(filter, FILTER_LEN, "src host 127.0.0.1 "
			"and dst host 127.0.0.2 and ip");
	}
	snprintf(device, DEVICE_LEN, LOOPBACK_DEV);

	/* start listening */
	sniffer_init(config, device, filter, config->loop_handler);
}

/*
 * start the internal and external threads.
 */
void
init_threads(uconfig * config)
{
	pthread_t p_ext, p_loop;

	pthread_create(&p_ext, NULL, (void *)external_init, config);
	pthread_create(&p_loop, NULL, (void *)loopback_init, config);

	/* TODO - handle signals and create some stop events, fatal() etc */
	while (1) usleep(100000);
}


