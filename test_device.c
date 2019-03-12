#include "block_device.h"

#define BLOCKSIZE 1024
int main() {
    char buf[BLOCKSIZE] = " hello there shiny block device";
    char readBuf[BLOCKSIZE];
    block_device_t t = createBlockDevice("foo.dev", BLOCKSIZE, 100);
    for (int j = 0; j < 10; j++) {
        buf[j] = '0' + j;
        writeBlock(t, j, buf);
        memset(readBuf, 0, BLOCKSIZE);
        readBlock(t, j, readBuf);
        for (int i = 0; i < BLOCKSIZE; i++){
            if (buf[i] != readBuf[i]) {
                printf( "buffer mismatch\n");
            }
        }
    }
    closeDevice(t);
    printf( "device tests complete\n");
}
