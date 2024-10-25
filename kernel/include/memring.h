#pragma once

#define MEMBLK_SIZE 16
#define MEMRING_LOC 0x12000
#define MEM_LOC (MEMRING_LOC * MEMBLK_SIZE)
#define MEMRING_LEN 1024 // 16k of mem free. don't try and increase this, youll prob wind up overwriting the os itself

#define FREE 0x00
#define BLK_START 0x01 // 0b01
#define IN_USE 0x02 // 0b10
#define BLK_END 0x04 // 0b100

// the memring sits at 0x12_000 whereas the memory it points to sits at 0x120_000 (<< 4)
// each byte in the memring points to a 16 byte block

unsigned char * memring = (unsigned char *) MEMRING_LOC;

void mem_init();
static inline char is_memring(void *ptr);
void * malloc(unsigned int len);
void free(void * ptr);
void * realloc(void *ptr, unsigned int len);