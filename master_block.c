#include <assert.h>

#include "master_block.h"


#define MYMAGIC 3735928559

// reads a master block from block device @ disk_location
// filling in the passed-in structure
// returns 0 on success, <0 on failure (e.g., magic doesn't match)
int readMasterBlock(BlockDevice *bd, MasterBlock *mb, disk_addr_t disk_location){
    assert(disk_location == 0 && "Invalid disk location\n");
    if (readFirstBytes(bd, (char*) mb, sizeof(MasterBlock)) != sizeof(MasterBlock)){
        fprintf(stderr, "readFirstBytes error\n");
        return -1;
    }
    if (mb->magic_number != MYMAGIC){
        fprintf(stderr, "Magic numbers do not match after reading block\n");
        return -1;
    }
    return 0;

}

//and

// writes master block pointed to by mb to the block device @ disk_location
// returns 0 on success, <0 if the write fails
int writeMasterBlock(BlockDevice *bd, MasterBlock *mb, disk_addr_t disk_location){
    assert(disk_location == 0 && "Invalid disk location\n");
    if (writeBlock(bd, 0, (char*) mb) != 0){
        fprintf(stderr, "Error writing block\n");
        return -1;
    }
    if (mb->magic_number != MYMAGIC){
        fprintf(stderr, "Magic numbers do not match after writing block\n");
        return -1;
    }
    return 0;
}

//you'll also want two helper functions,

// allocate memory for a Master Block and initialize its fields
// with the parameters to this function.
MasterBlock_p allocMasterBlock(int bytes_per_block, int number_of_blocks, disk_addr_t block_map_address){
    MasterBlock_p masterBlock = malloc(sizeof(MasterBlock));
    masterBlock->magic_number = MYMAGIC;
    masterBlock->bytes_per_block = bytes_per_block;
    masterBlock->number_of_blocks = number_of_blocks;
    masterBlock->disk_address = block_map_address;
    return masterBlock;
}



// free memory allocated in allocateMasterBlock
void freeMasterBlock(MasterBlock_p mb){

    free(mb);
}



int bootstrapDeviceFromMasterBlock(block_device_t main_bd){
    MasterBlock_p mb = allocMasterBlock(0,0,0);
    readMasterBlock(main_bd, mb, 0);
    setupBlockDevice(main_bd, mb->number_of_blocks, mb->bytes_per_block);
    return 0;
}


