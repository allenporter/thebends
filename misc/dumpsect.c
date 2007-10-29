// Dumps contents of a section in a mach-o binary
// ./dumpsect <binary> <segment> <section>
// allen@thebends.org
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <mach-o/loader.h>

int main(int argc, char ** argv) {
	FILE * fd;
	char *segname, *sectname;
	struct mach_header mh;

	fd = fopen(argv[1], "r");
	if (fread(&mh, sizeof(struct mach_header), 1, fd) != 1) {
		fprintf(stderr, "fread(): %s", strerror(errno));
		exit(0);
	}
	segname = argv[2];
	sectname = argv[3];

	int i;
	struct section * s;
	for (i=0; i < mh.ncmds; i++) {
		struct load_command lc;
		char * buf;

		if (fread(&lc, sizeof(struct load_command), 1, fd) != 1) {
			fprintf(stderr, "fread(): %s", strerror(errno));
			exit(0);
		}

		if (fseek(fd, 0-sizeof(struct load_command), SEEK_CUR) != 0) {
			fprintf(stderr, "fseek(): %s", strerror(errno));
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
   	        printf("SEGOFF:%s %x)\n", sc->segname, sc->fileoff);

		if (strcmp(segname, sc->segname)!=0) {
			free(buf);
			continue;
		}

		s = (struct section *)(sc+1);
		for (i=0; i < sc->nsects; i++) {
			printf("[%d] %x\n", i, sc->fileoff, s->offset);
			if (strcmp(sectname, s->sectname)==0) {
				free(buf);
				goto DONE;
			}
			s++;
		}
		s = NULL;
		free(buf);
	}

DONE:
	if (s == NULL)
		return 0;

	if (fseek(fd, s->offset, SEEK_SET) != 0) {
		fprintf(stderr, "fseek(): %s", strerror(errno));
		exit(0);
	}

	printf("%lx(%x) [OFF:%x]:\n", s->addr, s->size, s->offset);

	for (i=0; i < s->size; i++) {
		u_char b;
		fread(&b, sizeof(char), 1, fd);
		printf("%3x", b);
		if (i%8==0)
			printf("\n");
	}
	printf("\n");

	return 0;
}
