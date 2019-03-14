#pragma once

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "block_device.h"

typedef struct BlockMap{
    uint64_t magic;
    int count;
    char *buffer;
} BlockMap, *BlockMap_p;


BlockMap* allocBlockMap(int num_blocks);

void freeBlockMap(BlockMap* b);

void setBit(BlockMap* b, int i);

int clearBit(BlockMap* b, int i);

bool getBit(BlockMap* b, int i);

int allocItem(BlockMap* b);

int freeItem(BlockMap* b, int i);

BlockMap* readBlockMap(block_device_t bd, disk_addr_t disk_location);

int writeBlockMap(block_device_t bd, BlockMap *b, disk_addr_t disk_location);






