// List segments in a mach-o binary
// ./dump <binary>
// allen@thebends.org
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mach-o/loader.h>

int main(int argc, char ** argv) {
	FILE * fd;
	struct mach_header mh;

	fd = fopen(argv[1], "r");
	if (fread(&mh, sizeof(struct mach_header), 1, fd) != 1) {
		fprintf(stderr, "fread(): %s", strerror(errno));
		exit(0);
	}

	int i;
	for (i=0; i < mh.ncmds; i++) {
		struct load_command lc;
		char * buf;

		if (fread(&lc, sizeof(struct load_command), 1, fd) != 1) {
			fprintf(stderr, "fread(): %s", strerror(errno));
			exit(0);
		}
		if (fseek(fd, 0-sizeof(struct load_command), SEEK_CUR) != 0) {
			fprintf(stderr, "fread(): %s", strerror(errno));
			exit(0);
		}

		buf = (char *)malloc(sizeof(char)*lc.cmdsize);
		if (fread(buf, sizeof(char), lc.cmdsize, fd) != lc.cmdsize) {
			fprintf(stderr, "fread(): %s", strerror(errno));
			exit(0);
		}

		if (lc.cmd != LC_SEGMENT)
			continue;

		struct segment_command * sc = (struct segment_command *)buf;
		printf("%s\n", sc->segname);
		free(buf);
	}
	return 0;
}
