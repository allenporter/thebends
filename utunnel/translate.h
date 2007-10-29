#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

#include "utunnel.h"

typedef enum direction_info
{
	INCOMING = 1,
	OUTGOING
} direction;

/* a list of tunnel sessions */
typedef struct session_node
{
	endpoint external;
	endpoint internal;
	time_t last_usage;
} session;

session * session_lookup(endpoint * endp, direction dir);
session * session_create(endpoint * endp);

#endif /* !__TRANSLATE_H__ */
