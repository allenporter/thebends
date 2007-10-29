// Reads some data from the mach-o binary header
// ./read <binary>
// allen@thebends.org
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mach-o/loader.h>

void read_struct(FILE * fd, void * data, int datasize) {
	if (fread(data, datasize, 1, fd) != 1) {
		fprintf(stderr, "fread(): %s", strerror(errno));
		exit(0);
	}
}

void dump_sections(char * buf, int nsects) {
	int i;
	struct section * s = (struct section *)buf;

	for (i=0; i < nsects; i++) {
		printf("> SECTNAME: %s\n", s->sectname);
		printf("> SEGNAME: %s\n", s->segname);
		printf("> ADDR: 0x%lx\n", s->addr);
		printf("> SIZE: 0x%lx\n", s->size);
		printf("> OFFSET: 0x%lx\n", s->offset);
		printf("> ALIGN: %ld\n", s->align);
		printf("\n");
		s++;
	}
}

char * fttostr(unsigned long filetype) {
	char * ft;
	switch (filetype) {
		case MH_OBJECT:
			ft = "Object";
			break;
		case MH_EXECUTE:
			ft = "Executable";
			break;
		case MH_BUNDLE:
			ft = "Bundle";
			break;
		case MH_DYLIB:
			ft = "Dynamic Shared Library";
			break;
		case MH_PRELOAD:
			ft = "Preload";
			break;
		case MH_CORE:
			ft = "Core";
			break;
		case MH_DYLINKER:
			ft = "Dynamic Linker Shared LIbrary";
			break;
		default:
			ft = "";
			break;
	}
	return ft;
}

char * cmdtostr(unsigned long cmd) {
	char *p;
	switch (cmd) {
		case LC_SEGMENT:
			p="SEGMENT";
			break;
		case LC_SYMTAB:
			p="SYMTAB";
			break;
		case LC_DYSYMTAB:
			p="DYSYMTAB";
			break;
		case LC_THREAD:
			p="THREAD";
			break;
		case LC_UNIXTHREAD:
			p="UNIXTHREAD";
			break;
		case LC_LOAD_DYLIB:
			p="LOAD_DYLIB";
			break;
		case LC_ID_DYLIB:
			p="ID_DYLIB";
			break;
		case LC_PREBOUND_DYLIB:
			p="PREBOUND_DYLIB";
			break;
		case LC_LOAD_DYLINKER:
			p="LOAD_DYLINKER";
			break;
		case LC_ID_DYLINKER:
			p="ID_DYLINKER";
			break;
		case LC_ROUTINES:
			p="ROUTINES";
			break;
		case LC_TWOLEVEL_HINTS:
			p="TWOLEVEL_HINTS";
			break;
		case LC_SUB_FRAMEWORK:
			p="SUB_FRAMEWORK";
			break;
		case LC_SUB_UMBRELLA:
			p="SUB_UMBRELLA";
			break;
		case LC_SUB_LIBRARY:
			p="SUB_LIBRARY";
			break;
		case LC_SUB_CLIENT:
			p="SUB_CLIENT";
			break;
		default:
			p="N/A";
	}
	return p;
}

void dump_loadcommand(char * buf, struct load_command lc)
{
	if (lc.cmd == LC_SEGMENT) {
		struct segment_command * sc = (struct segment_command *)buf;

		printf("SEGNAME: %s\n", sc->segname);
		printf("VMADDR: 0x%lx\n", sc->vmaddr);
		printf("VMSIZE: 0x%lx\n", sc->vmsize);
		printf("FileOff: 0x%lx\n", sc->fileoff);
		printf("FileSize: 0x%lx\n", sc->filesize);
		printf("NSects: %ld\n", sc->nsects);
		printf("\n");
		dump_sections((char *)(sc+1), sc->nsects);
	} else if (lc.cmd == LC_LOAD_DYLINKER) {
		struct dylinker_command * dc = (struct dylinker_command *)buf;
		char * dynlink = buf + dc->name.offset;
		printf("DynLinker: %s\n", dynlink);
	} else if (lc.cmd == LC_LOAD_DYLIB) {
		struct dylib_command * dc = (struct dylib_command *)buf;
		char * dylib = buf + dc->dylib.name.offset;
		printf("DynLib: %s\n", dylib);
	} else if (lc.cmd == LC_PREBOUND_DYLIB) {
		struct prebound_dylib_command * pdc =
			(struct prebound_dylib_command *)buf;
		char *shlib = buf + pdc->name.offset;	
		printf("Num Modules: %ld\n", pdc->nmodules);
		printf("Shared Lib: %s\n", shlib);
	} else if (lc.cmd == LC_SYMTAB) {
		struct symtab_command * sc =
			(struct symtab_command *)buf;
		printf("Symbol Offset: 0x%lx\n", sc->symoff);
		printf("Symbols: %ld\n", sc->nsyms);
		printf("String Table Offset: 0x%lx\n", sc->stroff);
		printf("String Table Offset: %ld\n", sc->stroff);
		printf("String Table Size: 0x%lx\n", sc->strsize);
	} else {
		printf("Skipping struct\n");
	}
	printf("\n");
}

int main(int argc, char ** argv) {
	FILE * fd;
	struct mach_header mh;

	fd = fopen(argv[1], "r");
	if (fread(&mh, sizeof(struct mach_header), 1, fd) != 1) {
		fprintf(stderr, "fread(): %s", strerror(errno));
		exit(0);
	}

	printf("MAGIC: %lx\n", mh.magic);
	printf("CPUTYPE: %s\n", mh.cputype==CPU_TYPE_POWERPC?"PowerPC":"x86");
	printf("CPUSUBTYPE: %x\n", mh.cpusubtype);
	printf("FILETYPE: %s\n", fttostr(mh.filetype));
	printf("\n");

	int i;
	for (i=0; i < mh.ncmds; i++) {
		struct load_command lc;
		char * buf;

		if (fread(&lc, sizeof(struct load_command), 1, fd) != 1) {
			fprintf(stderr, "fread(): %s", strerror(errno));
			exit(0);
		}

		printf("CMD: %s\n", cmdtostr(lc.cmd));
		printf("CMD SIZE: 0x%lx\n", lc.cmdsize);

		if (fseek(fd, 0-sizeof(struct load_command), SEEK_CUR) != 0) {
			fprintf(stderr, "fread(): %s", strerror(errno));
			exit(0);
		}

		buf = (char *)malloc(sizeof(char)*lc.cmdsize);
		read_struct(fd, buf, lc.cmdsize * sizeof(char));
		dump_loadcommand(buf, lc);
		free(buf);
	}
	return 0;
}
