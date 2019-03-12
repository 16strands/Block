// API for raw access to a block device.
// For the class, the block device is going to be represented by a
// file on the host operating system. With very minor tweaks, it can
// be directed to a real block device on a Unix system.

// Methods:
// open - opens a block device
// close - closes the device
// read_block - takes a block number and a buffer - reads that block into the buffer
// write_block - takes a block number and a buffer - writes the block

// Public attributes:
// block_size - number of bytes per block on this device
// block_count - number of blocks on this device

#include "block_device.h"

// This method creates a new block device, possibly creating a new
// file, truncating or exending an existing one.
// deviceName is the name of the file being created.
// blockSize is the bytes per block
// blockCount is the number of blocks for the device
block_device_t createBlockDevice(char * deviceName, int blockSize, int blockCount) {
    block_device_t ret = malloc(sizeof(BlockDevice));
    ret->m_bytesPerBlock = blockSize;
    ret->m_blockCount = blockCount;
    ret->m_deviceHandle = open(deviceName, O_RDWR | O_CREAT, 0644);
    // octcal 644 is owner read/write, everyone else read-only - see 'man chmod'
    if (ret->m_deviceHandle < 0) {
        char *desc = strerror(errno);
        fprintf(stderr, "error creating device file %s due to %s\n", deviceName, desc);
        return NULL;
    }
    int truncResult = ftruncate(ret->m_deviceHandle, blockSize * blockCount);
    if (truncResult != 0) {
        fprintf(stderr, "error setting file size\n");
        return NULL;
    }
    return ret;
}

// closes a device, frees the memory for the BlockDevice structure
void closeDevice(block_device_t bd) {
    if (bd->m_deviceHandle) {
        int ok = close(bd->m_deviceHandle);
        if (ok != 0) {
            fprintf(stderr, "error closing device %s\n", strerror(errno));
        } else {
            bd->m_deviceHandle = 0;
        }
    }
    free(bd);
}

// reads data into memory pointed to by buffer from the block device at blockNum
// Returns 0 on success, -1 on failure
int readBlock(block_device_t bd, int blockNum, char *buff) {
    int byteOffset = blockNum * bd->m_bytesPerBlock;
    int seekRet = lseek(bd->m_deviceHandle, byteOffset, SEEK_SET);
    if (seekRet != byteOffset) {
        fprintf(stderr, "seek error in readBlock\n");
        return -1;
    }
    int numRead = read(bd->m_deviceHandle, buff, bd->m_bytesPerBlock);
    if (numRead != bd->m_bytesPerBlock) {
        // raise some heck here - should never happen!
        fprintf(stderr, "read error in readBlock\n");
        return -1;
    }
    return 0;
}

// writes data pointed to by buffer to the block device at blockNum
// address. Returns 0 on success, -1 on failure
int writeBlock(block_device_t bd, int blockNum, const char *buff) {
    int byteOffset = blockNum * bd->m_bytesPerBlock;
    int seekRet = lseek(bd->m_deviceHandle, byteOffset, SEEK_SET);
    if (seekRet != byteOffset) {
        fprintf(stderr, "seek error in writeBlock\n");
        return -1;
    }
    int numWritten = write(bd->m_deviceHandle, buff, bd->m_bytesPerBlock);
    if (numWritten != bd->m_bytesPerBlock) {
        fprintf(stderr, "write error in readBlock\n");
        return -1;
    }
    return 0;
}
