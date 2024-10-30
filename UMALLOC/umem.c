#include "umem.h"      // Include header file for allocator definitions
#include <stdio.h>     // For perror, fprintf (error handling), and printf (for debugging)
#include <stdlib.h>    // For exit (in case of memory corruption)
#include <fcntl.h>     // For open (when accessing /dev/zero for mmap)
#include <unistd.h>    // For close (to close file descriptor)
#include <sys/mman.h>  // For mmap and associated memory management constants

int umeminit (size_t sizeOfRegion, int allocationAlgo){ // used to initalize the memory using mmap()
    // open the /dev/zero device
    int fd = open("/dev/zero", O_RDWR);

    // sizeOfRegion (in bytes) needs to be evenly divisible by the page size
    void *ptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) { perror("mmap"); exit(1); }

    // close the device (don't worry, mapping should be unaffected)
    close(fd);
    return 0;
}

void *umalloc(size_t size){


}

int ufree(void *ptr){


}

void *urealloc(void *ptr, size_t size){


}

void umemstats(void){


}