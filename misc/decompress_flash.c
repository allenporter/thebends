// flash_decompress.c
// Author: Allen Porter <allen@thebends.org>
// For decompressing flash files (header bytes start with CWS)
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <zlib.h>

struct header {
  char id[3];
  char version;
  uint32_t size;
};

#define M_32_SWAP(a) {              \
  u_int32_t _tmp = a;           \
  ((char *)&a)[0] = ((char *)&_tmp)[3];       \
  ((char *)&a)[1] = ((char *)&_tmp)[2];       \
  ((char *)&a)[2] = ((char *)&_tmp)[1];       \
  ((char *)&a)[3] = ((char *)&_tmp)[0];       \
}

int main(int argc, char* argv[]) {
  assert(argc == 2);
  int fd = open(argv[1], O_RDONLY, 0);
  assert(fd > 0);
  struct stat sb;
  int ret = fstat(fd, &sb);
  assert(ret != -1);
  assert(sb.st_size > 0);

  unsigned char* buf = malloc(sizeof(char) * sb.st_size);

  static z_stream z = { 0 };
  ret = read(fd, buf, sb.st_size);
  assert(ret == sb.st_size);
  close(fd);

  struct header* h = (struct header*)buf;
  assert(h->id[0] == 'C');
  assert(h->id[1] == 'W');
  assert(h->id[2] == 'S');
  assert(h->version >= 6);
  M_32_SWAP(h->size);

  z.next_in = buf + sizeof(struct header);
  z.avail_in = sb.st_size - sizeof(struct header);
  unsigned char *out = malloc(sizeof(char) * h->size);
  z.next_out = out;
  z.avail_out = h->size;

  ret = inflateInit(&z);
  if (z.msg != NULL) {
    fprintf(stderr, "infateInit: %s\n", z.msg);
  }
  assert(ret == Z_OK);
  ret = inflate(&z, Z_FINISH);
  if (z.msg != NULL) {
    fprintf(stderr, "infate: %s\n", z.msg);
  }
  assert(ret == Z_STREAM_END);
  ret = inflateEnd(&z);
  if (z.msg != NULL) {
    fprintf(stderr, "infateEnd: %s\n", z.msg);
  }
  assert(ret == Z_OK);

  fd = open("out.swf", O_WRONLY | O_CREAT, 0755);
  assert(fd > 0);
  ret = write(fd, out, h->size);
  if (ret == -1) {
    err(1, "write");
  }
  assert(ret == h->size);
  close(fd);
  free(out);
  free(buf);
  return 0;
}
