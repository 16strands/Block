
#include "block_map.h"

#define MYMAGIC2 4277006349


BlockMap* allocBlockMap(int num_blocks){ // allocates and returns a pointer to a BlockMap
    int size = ceil(num_blocks/8);
    BlockMap_p blockmap = malloc(sizeof(BlockMap));
    blockmap->magic = MYMAGIC2;
    blockmap->buffer = malloc(size);  // better name for size
    blockmap->count = num_blocks;
    return blockmap;
}

void freeBlockMap(BlockMap* b){ // frees a BlockMap
    free(b->buffer); 
    free(b);
}

void setBit(BlockMap* b, int i){ // sets bit i of the BlockMap
    int byte = (i/8);
    b->buffer[byte] |= (1 << i%8);
}

int clearBit(BlockMap* b, int i){ // clears bit i of the BlockMap
    int byte = (i/8);
    b->buffer[byte] &= ~(1 << i%8);
    return 0;
}

bool getBit(BlockMap* b, int i){ // returns the bit (boolean) value of the ith bit of the BlockMap
    int byte = (i/8);
    return (b->buffer[byte] & ( 1 << i%8 ));
}


int allocItem(BlockMap* b){ // returns the index of an item that was free, but is now allocated. 
// If there are no more free items, returns a negative number
    for (int i=0; i < b->count; i++){
        if (getBit(b, i) == 0){
            setBit(b, i);
            return i;
        }
    }
    return -1;
}


int freeItem(BlockMap* b, int i){ // frees the item at location i, first checking that it wasn't already free. 
// Returns 0 on success, <0 if there was an error
    if (getBit(b,i) == 1){
        clearBit(b,i);
        return 0;
    }
    return -1;
}

BlockMap* readBlockMap(block_device_t bd, disk_addr_t disk_location){ // read a BlockMap from the block device
    BlockMap_p b = allocBlockMap(bd->m_blockCount);
    int bytes_in_blockmap = ceil(bd->m_blockCount/8.0);
    int blocks_to_store_blockmap = ceil((1.0*bytes_in_blockmap)/bd->m_bytesPerBlock);
    for (int i = 0; i < blocks_to_store_blockmap; i++){
        assert(readBlock(bd, i+disk_location, &(b->buffer[i*bd->m_bytesPerBlock]))==0 && "Error reading from block device\n");
    }
    b->count = bd->m_blockCount;
    return b;
}

int writeBlockMap(block_device_t bd, BlockMap *b, disk_addr_t disk_location){ // write a BlockMap to the block device
    // bits persist til here 
    int bytes_in_blockmap = ceil(bd->m_blockCount/8.0);
    int blocks_to_store_blockmap = ceil((1.0*bytes_in_blockmap)/bd->m_bytesPerBlock);
    for (int i = 0; i < blocks_to_store_blockmap; i++){
        assert(writeBlock(bd, i+disk_location, &b->buffer[i*bd->m_bytesPerBlock])==0 && "Error writing to block device\n");
    }
    return 0;

}








