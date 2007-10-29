// telephony.c
// Author: Allen Porter <allen@thebends.org>
//
// Based on work by the iphone-wireless/CellStumbler guys and disassembling
// FieldTest.app.
#include <CoreTelephony/CoreTelephony.h>
#include <unistd.h>

void Dump(void* x, int size) {
  char* c = (char*)x;
  int i;
  for (i = 0; i < size; i++) {
    printf(" %x ", c[i]);
  }
  printf("\n");
}

void CreateCallback() {
  printf("Foo! called\n");
}

int main() {
  printf("Create\n");

  char c[255];
  memset(c, 0, 255);

  CTServerConnectionRef conn = _CTServerConnectionCreate(
    kCFAllocatorDefault, CreateCallback, NULL);
  if (conn == 0) {
    fprintf(stderr, "_CTServerConnectionCreate == 0\n");
    return 1;
  }
  printf("conn=%d\n", conn);
  Dump(conn, sizeof(struct __CTServerConnection));

  int port = _CTServerConnectionGetPort(conn);
  printf("port=%d\n", port);
  CFMachPortRef mach_port = CFMachPortCreateWithPort(kCFAllocatorDefault, port,
      NULL, NULL, NULL);
  printf("mach_port=%x\n", CFMachPortGetPort(mach_port));

  _CTServerConnectionCellMonitorStart(mach_port, conn);

  while (1) {
    int count = 0;
    _CTServerConnectionCellMonitorGetCellCount(mach_port, conn, &count);

    int i;
    for (i = 0; i < count; i++) {
      CellInfoRef cellinfo;
      _CTServerConnectionCellMonitorGetCellInfo(mach_port, conn, i, &cellinfo);
      printf("Cell site: %d, MNC: %d ", i, cellinfo->servingmnc);
      printf("Location: %d, Cell ID: %d, Station: %d, ", cellinfo->location,
             cellinfo->cellid, cellinfo->station);
      printf("Freq: %d, RxLevel: %d, ", cellinfo->freq, cellinfo->rxlevel);
      printf("C1: %d, C2: %d\n", cellinfo->c1, cellinfo->c2);
    }
    printf("\n");
    sleep(1);
  }
}
