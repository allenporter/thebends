/*
 * interfaces.c
 *
 * Prints out ethernet interfaces.  Some code is borrowed from *BSD ifconfig.
 * allen@thebends.org
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sysctl.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <string.h>

int main(int argc, char * argv[])
{
	int mib[6];
	size_t len;
	int8_t *buf, *next, *end;
	struct if_msghdr *ifm;
	struct sockaddr_dl *sdl;

	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	mib[5] = 0;

	if (sysctl(mib, 6, NULL, &len, NULL, 0) == -1)
	{
		fprintf(stderr, "%s(): sysctl(): %s\n",
				__func__, strerror(errno));
		exit(1);
	}

	buf = (char *)malloc(len);
	if (buf == NULL)
	{
		fprintf(stderr, "%s(): malloc(): %s\n",
				__func__, strerror(errno));
		exit(1);
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) == -1)
	{
		fprintf(stderr, "%s(): sysctl(): %s\n",
				__func__, strerror(errno));
		exit(1);
	}
	end = buf + len;

	for (next = buf; next < end; next += ifm->ifm_msglen)
	{
		ifm = (struct if_msghdr *)next;
		if (ifm->ifm_type == RTM_IFINFO)
		{
			char buf[BUFSIZ];
			sdl = (struct sockaddr_dl *)(ifm + 1);
			strncpy(buf, sdl->sdl_data, sdl->sdl_nlen);
			buf[sdl->sdl_nlen] = NULL;
			if (sdl->sdl_type == IFT_ETHER)
			{
				printf("%s\n", buf);
			}
		}
	}
	free(buf);

	return 0;
}
