#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


/*
    The BlockDevice is the API that a file system is built on.
    Any block device supports block-level reads and writes.

    In this system, we simulate a block device using an OS file.
    In theory (an exercise for the motivated?) we could use almost
    this exact code to access an actual raw device (e.g.,
    /dev/rdiskx), but then the program would need to run as
    super-user, and if the wrong device were specified, you could
    overwrite your OS or user data. Beware.

    API overview:

    The BlockDevice constructor, when given three arguments, creates
    a new, empty block device. The two-argument version of the
    constructor opens the file given as its first argument.
    The blockSize argument specifies how many bytes there are per
    block. Real world file systems have had blocks as small as 512
    bytes, and as large as 4096 bytes.

    The action end of a block device are the readBlock and writeBlock
    calls. They both take a block number and a buffer. readBlock fills
    the buffer with the data from the block device @ blockNum, and
    writeBlock writes the buffer to the block device @ blockNum.

    The m_bytesPerBlock and m_blockCount members are available for
    friends, like the BlockMap.
*/

#define ceilDiv(n,d) ( (int)((n + d - 1)/d))

typedef int disk_addr_t;  // signed int, so we can have negative
                          // values for, e.g., copy-on-write

typedef struct BlockDevice {
    int m_deviceHandle;
    int m_blockCount;
    int m_bytesPerBlock;
} BlockDevice, *block_device_t;

block_device_t createBlockDevice(char * deviceName, int blockSize, int blockCount);

void closeDevice(block_device_t bd);

int readBlock(block_device_t bd, int blockNum, char *buff);

int writeBlock(block_device_t bd, int blockNum, const char *buff);
