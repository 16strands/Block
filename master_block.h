#pragma once

#include <unistd.h>

#include "block_device.h"

typedef struct MasterBlock{
    uint64_t magic_number;
    uint64_t bytes_per_block;
    uint64_t number_of_blocks;
    uint64_t disk_address;

}MasterBlock, *MasterBlock_p;

int readMasterBlock(BlockDevice *bd, MasterBlock *mb, disk_addr_t disk_location);

int writeMasterBlock(BlockDevice *bd, MasterBlock *mb, disk_addr_t disk_location);

MasterBlock_p allocMasterBlock(int bytes_per_block, int number_of_blocks, disk_addr_t block_map_address);

void freeMasterBlock(MasterBlock_p mb);

int bootstrapDeviceFromMasterBlock(block_device_t main_bd);













