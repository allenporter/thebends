/*
 * utunnel
 * Copyright (c) 2004 Allen Porte <allen@thebends.org>
 *
 * translate.c
 */
#include "translate.h"

session session_table[MAX_CLIENTS];

/*
 * initialize the session table
 */
int
session_init_table()
{
	u_char localip[4] = { 127, 0, 0, 0 };
	session * p;
	int i;

	/* initialize random loopback addresses */
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		p = &session_table[i];
		localip[3] = i + 2;
		memset(&p->external, 0, sizeof(endpoint));
		memcpy(&p->internal.addr, localip, sizeof(struct in_addr));
		p->internal.port = 0;
	}
}

/*
 * Looks for the first empty slot and if none is found, uses the oldest slot
 * in the table.
 */
int
find_empty_slot()
{
	int i;
	int min_slot;
	time_t min_time = 0;

	/*
	 * this O(n) search method could be improved
	 */
	for (i=0; i < MAX_CLIENTS; i++)
	{
		time_t cur_time = session_table[i].last_usage;

		/* empty slot, return this index */
		if (cur_time == 0)
			return i;

		/* keep track of least recently used */
		if (cur_time < min_time || min_time == 0)
		{
			min_time = cur_time;
			min_slot = i;
		}
	}

	/* return least recently used slot */
	return min_slot;
}

/*
 * creates a new session and returns the translated loopback ip address
 */
session *
session_create(endpoint * external)
{
	endpoint * internal;
	session * new_node;
	int new_slot;

	if (external == NULL)
	{
		fprintf(stderr, "trans_insert(): NULL argument\n");
		exit(1);
	}

	/* locate an empty session slot */
	new_slot = find_empty_slot();
	new_node = &session_table[new_slot];

	/* assign the new ip address/port, update last usage time */	
	memcpy(&new_node->external, external, sizeof(endpoint));
	new_node->last_usage = time(NULL);

printf("%d. Write session (%s:%d) & ", new_slot,
	inet_ntoa(new_node->external.addr),
	new_node->external.port);
printf("(%s:%d)\n", inet_ntoa(new_node->internal.addr),
	new_node->internal.port);

	/* return the loopback ip address */
	return new_node;
}

/*
 * lookup a session
 */
session *
session_lookup(endpoint * endp, direction dir)
{
	int i;

	/* validate arguments */
	if (endp == NULL)
	{
		fprintf(stderr, "trans_lookup: NULL pointer argument");
		exit(1);
	}

	for (i=0; i < MAX_CLIENTS; i++)
	{
		endpoint * s;		/* current endpoint */

		if (dir == INCOMING)
		{
			s = &session_table[i].external;
			if (memcmp(&endp->addr, &s->addr,
					sizeof(struct in_addr))==0 &&
					endp->port == s->port)
				return &session_table[i];
		}
		else
		{
			s = &session_table[i].internal;
			if (memcmp(&endp->addr, &s->addr,
					sizeof(struct in_addr))==0)
				return &session_table[i];
		}

	}

	return NULL;			/* no match */
}
