#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/disk.h>
#include <strings.h>
#include <iostream>
#include <architecture/byte_order.h>

using namespace std;

struct Block0 {
    unsigned short     sbSig;      /* unique value for SCSI block 0 */
    short     sbBlkSize;  /* block size of device */
    long      sbBlkCount; /* number of blocks on device */
    short     sbDevType;  /* device type */
    short     sbDevId;    /* device id */
    long      sbData;     /* not used */
    short     sbDrvrCount;    /* driver descriptor count */
    short     sbMap[247]; /* descriptor map */
};

static void swap(Block0* block) {
  block->sbSig = OSSwapConstInt16(block->sbSig);
  block->sbBlkSize = OSSwapConstInt16(block->sbBlkSize);
  block->sbBlkCount = OSSwapConstInt32(block->sbBlkCount);
  block->sbDevType = OSSwapConstInt16(block->sbDevType);
  block->sbDevId = OSSwapConstInt16(block->sbDevId);
  block->sbData = OSSwapConstInt32(block->sbData);
  block->sbDrvrCount = OSSwapConstInt16(block->sbDrvrCount);
}

int main(int argc, char* argv[]) {
  int fd;
  if (argc != 2) {
    fprintf(stderr, "usage: %s <raw disk>\n", argv[0]);
    exit(1);
  }

  if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
    perror("open");
    exit(1);
  }

  off_t offset = lseek(fd, 0, 0);

  struct Block0 block;
  bzero((char*)&block, 512);
  if (read(fd, (char*)&block, sizeof(struct Block0)) != sizeof(struct Block0)) {
    perror("read");
    exit(1);
  }
  swap(&block);

  cout << "Signature: " << block.sbSig << endl;
  cout << "Block size: " << block.sbBlkSize << endl;
  cout << "Blocks: " << block.sbBlkCount << endl;
  cout << "Device Type: " << block.sbDevType << endl;
  cout << "Device ID: " << block.sbDevId << endl;
  cout << "Data: " << block.sbData << endl;
  cout << "Driver Count: " << block.sbDrvrCount << endl;
  cout << "Descriptor Map: " << endl;
  for (int i = 0; i < 247; ++i) {
    cout << " " << hex << block.sbMap[i];
  }
  cout << endl;

  close(fd);
  return 0;
}
