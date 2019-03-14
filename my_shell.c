#include "block_device.h"
#include "master_block.h"
#include "block_map.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <stdbool.h>

#define OPTIONS "Options:\n" \
"   newfs [filesystem name] [bytes per block] [number of blocks] - create a new filesystem\n" \
"   mount [filesystem name] - mount filesystem\n" \
"   unmount - unmount currently mounted filesystem\n" \
"   allocBlock - allocates a block and returns index of that block\n" \
"   freeBlock [index] - frees block at specified index\n" \
"   blockMap - prints blockMap status of currently mounted filesystem\n"\
"   help - display this help menu\n" \
"   quit - exit the program\n"

// returns true if a is a prefix of b
bool prefix(char *a, char *b) {
    return !(strncmp(a,b,strlen(a)));
}

// breakWords takes an input string and an array of pointers to strings.
// It sets each element of the array to the next word, and returns the
// total number of words parsed. max_words is the length of the array
// of pointers.
int breakWords(char *in, char **out, int max_words) {
    const char *delim = " \t"; // separating tokens
    // the first time we call strtok, we give it the buffer,
    // it returns the "newfs" command that got us here
    char *words = strtok(in, delim);
    int i = 0;
    for (; i < max_words; i++) {
        out[i] = words;
        if (words == NULL) break;
        // each subsequent call to strtok, we pass NULL (it keeps
        // a pointer into our string (!), and returns a pointer to
        // the next token, which we hope is the filename this time
        words = strtok(NULL, delim);
    }
    return i;
}

int main(int argc, char** argv) {
    printf("Welcome! You can exit by pressing Ctrl+C at any time...\n");

    bool done = false;
    char* buf;
    MasterBlock_p main_mb = NULL; // the master block
    block_device_t main_bd = NULL; // the block device
    BlockMap_p       main_bs = NULL; // the blockmap (a bitset)
    const int MAX_ARGS = 32;
    char *arguments[MAX_ARGS];

    while (!done && (buf = readline(">> ")) != NULL) {
        if (strlen(buf) > 0) {
            add_history(buf);
        }

        if (prefix("quit", buf)) {
            done = true;
        } else if (prefix("newfs", buf)) {
            int num_words = breakWords(buf, arguments, MAX_ARGS);
            block_device_t bd = NULL;
            if (num_words == 4) {
                char *device_name = arguments[1];
                // atoi is ASCII -> integer, so get the block size
                // into an integer.
                int block_size = atoi(arguments[2]);
                // and now read the number of blocks
                int block_count = atoi(arguments[3]);
                // now we know everything we need to create
                // the device.
                bd = createBlockDevice(device_name, block_size, block_count);
                // allocate a fresh Master Block structure
                MasterBlock_p mb = allocMasterBlock(block_size, block_count, 0);
                BlockMap_p bs = allocBlockMap(block_count);
                int bytes_in_blockmap = ceilDiv((double)block_count,8.0);
                int blocks_to_store_blockmap = ceilDiv(bytes_in_blockmap,block_size);
                // write the master block to the device
                for( int i = 0; i < blocks_to_store_blockmap + 1; i++){
                  setBit(bs, i);
                }
                writeMasterBlock(bd, mb, 0);
                writeBlockMap(bd, bs, 1);
                closeBlockDevice(bd);
                //bits persist til here
                printf("created %s with %d blocks of %d bytes per block\n", device_name, block_count, block_size);

            }
            if (bd == NULL) {
                // if any of the strtok calls failed, we end up here,
                // so tell the user what we expected
                fprintf(stderr, "usage: newfs <filename> <bytes per block> <block count>\n");
            }
        } else if (prefix("mount", buf)) {
            int num_words = breakWords(buf, arguments, MAX_ARGS);
            char *device_name = arguments[1];        // this should be the device name
            if (num_words == 2 && device_name != NULL) {
                main_bd = openBlockDevice(device_name);
                if (bootstrapDeviceFromMasterBlock(main_bd) >= 0) {
                    // now main_bd points to a working, initialized block device
                    main_mb = allocMasterBlock(0,0,0); // allocate a blank Master Block
                    // now fill it in
                    if (readMasterBlock(main_bd, main_mb, 0) == 0 &&
                        ((main_bs = readBlockMap(main_bd, 1)) != NULL)) {
                        printf("%s successfully mounted\n", device_name);
                        // for (int i = 0; i<main_bd->m_blockCount; i++){
                        //   printf("this bit in mount: %d\n", getBit(main_bs,i) );
                        // }
                    } else {
                        fprintf(stderr, "mount failed. Corrupt device?\n");
                        main_bd = NULL;
                        main_mb = NULL;
                        main_bs = NULL;
                    }
                } else {
                    fprintf(stderr, "mount failed. Was that the right filename?\n");
                    main_bd = NULL;
                }
            } else {
                fprintf(stderr, "usage: mount <device file name>\n");
            }
        } else if (prefix("unmount", buf)) {
            if (main_bd != NULL) {
                // write the block map to disk
                int writeZero = writeBlockMap(main_bd, main_bs, 1);
                // close the file
                int closeZero = closeBlockDevice(main_bd);
                // set the mb to NULL
                main_bd = NULL;
                main_mb = NULL;
                if (writeZero == 0 && closeZero == 0) {
                    printf("unmount successful\n");
                } else {
                    fprintf(stderr, "unmount error\n");
                }
            } else {
                fprintf(stderr, "you need to mount before unmounting\n");
            }
        } else if (prefix("allocBlock", buf)) {
            if (main_bs != NULL) {
                int ba = allocItem(main_bs);
                if (ba >= 0) {
                    printf("allocated %d\n", ba);
                } else {
                    printf("unable to allocate block\n");
                }
            } else {
                fprintf(stderr, "you need to mount a filesystem first\n");
            }
        } else if (prefix("freeBlock", buf)) {
            int num_words = breakWords(buf, arguments, MAX_ARGS);
            char *btf_char = arguments[1];
            if (num_words == 2) {
                if (main_bs != NULL) {
                    int block_to_free = atoi(btf_char);
                    freeItem(main_bs, block_to_free);
                } else {
                    fprintf(stderr, "you need to mount a filesystem first\n");
                }
            } else {
                fprintf(stderr, "usage: freeBlock <blocknum>");
            }
        } else if (prefix("blockMap", buf)) {
            if (main_mb != NULL) {
                printf("%d blocks in Block Map\n", main_bs->count);
                for (int i = 0; i < main_bs->count; i++) {
                    putchar(getBit(main_bs, i) ? '1' : '0');
                }
                printf("\n");
            } else {
                fprintf(stderr, "you need to mount a filesystem first\n");
            }
        } else if (prefix("echo", buf)) {
            printf("%s\n", &buf[5]);
        } else if (prefix("help", buf)) {
            printf(OPTIONS);
        }

        // readline malloc's a new buffer every time.
        free(buf);
    }

    return 0;
}

