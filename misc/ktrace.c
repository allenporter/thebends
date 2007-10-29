#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/ktrace.h>
#include <string.h>
#include <errno.h>

int main(int argc, char * argv[]) {
	char * filename;
	int pid;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <pid> <tracefile>\n", argv[0]);
		exit(0);
	}

	pid = (int)strtol(argv[1], (char**)NULL, 10);
	filename = argv[2];

	printf("Logging process %d to %s\n", pid, filename);

	if (ktrace(filename, KTROP_SET, KTRFAC_SYSCALL, pid) == -1) {
		fprintf(stderr, "ktrace(): %s\n", strerror(errno));
		exit(1);
	}
	if (ktrace(filename, KTROP_SET, KTRFAC_SYSRET, pid) == -1) {
		fprintf(stderr, "ktrace(): %s\n", strerror(errno));
		exit(1);
	}
	return 0;
}
