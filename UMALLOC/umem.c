#include "umem.h"      // Include header file for allocator definitions
#include <stdio.h>     // For I/O
#include <string.h>    // For strings
#include <stdlib.h>    // For exit (in case of memory corruption)
#include <fcntl.h>     // For open (when accessing /dev/zero for mmap)
#include <unistd.h>    // For close (to close file descriptor)
#include <sys/mman.h>  // For mmap and associated memory management constants

// Global Variable initialization
void *base_ptr = NULL; 
int allocAlgo = 0;
size_t heapSize = 0;
node_t *freeList = NULL;
static node_t *lastAlloc = NULL; // for NEXT FIT
size_t total_allocations = 0;    // Tracks the total number of allocations
size_t totalDeallocations = 0;  // Tracks the total number of deallocations

// Prototyping helper functions
void *best_fit(size_t size); 
void *worst_fit(size_t size);
void *first_fit(size_t size);
void *next_fit(size_t size);
void blockAllocated(void *block, size_t size);
void addToFreeList(node_t *header);
void coalesce();
size_t calculateFragmentation();

int umeminit(size_t sizeOfRegion, int allocationAlgo) {
    if (base_ptr != NULL || sizeOfRegion <= 0) {
        return -1;  // Return failure if already initialized or if size is invalid
    }

    size_t pageSize = getpagesize();
    sizeOfRegion = (sizeOfRegion + pageSize - 1) & ~(pageSize - 1);

    base_ptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    allocAlgo = allocationAlgo;
    heapSize = sizeOfRegion;

    // Initialize the free list to cover the entire region, leaving room for the header
    freeList = (node_t *)base_ptr;
    freeList->size = sizeOfRegion - sizeof(header_t);
    freeList->next = NULL;

    return 0;
}


void *umalloc(size_t size) {
    if (base_ptr == NULL) {
        return NULL;  // Ensure umeminit() is called first
    }

    // Align requested size to 8 bytes and add header size
    size = (size + 7) & ~7;
    size += sizeof(header_t);  // Account for header in allocation size

    void *allocated_block = NULL;
    switch (allocAlgo) {
        case BEST_FIT:
            allocated_block = best_fit(size);
            break;
        case WORST_FIT:
            allocated_block = worst_fit(size);
            break;
        case FIRST_FIT:
            allocated_block = first_fit(size);
            break;
        case NEXT_FIT:
            allocated_block = next_fit(size);
            break;
        default:
            fprintf(stderr, "Error: Invalid allocation algorithm.\n");
            return NULL;
    }

    if (allocated_block == NULL) {
        return NULL;  // No suitable block found
    }

    // Mark block as allocated and update total_allocations
    blockAllocated(allocated_block, size - sizeof(header_t));
    total_allocations++; 
    return (void *)((char *)allocated_block + sizeof(header_t));  // Return pointer after header
}


int ufree(void *ptr){
    if (ptr == NULL) return 1;

    header_t *header = (header_t *)((char *)ptr - sizeof(header_t));
    if (header->magic != MAGIC) {
        fprintf(stderr, "Error: Memory corruption detected at block %p\n", ptr);
        exit(1);
    }

    addToFreeList((node_t *)header);
    coalesce();
    totalDeallocations++;  // Increment total deallocations
    return 1;
}

void *urealloc(void *ptr, size_t size){
    if (ptr == NULL) return umalloc(size);
    if (size == 0) {
        ufree(ptr);
        return NULL;
    }

    size = (size + 7) & ~7;

    header_t *header = (header_t *)((char *)ptr - sizeof(header_t));
    if (header->magic != MAGIC) {
        fprintf(stderr, "Error: Memory corruption detected at block %p\n", ptr);
        exit(1);
    }

    if (header->size >= size) return ptr;

    void *new_block = umalloc(size);
    if (new_block == NULL) return NULL;

    memcpy(new_block, ptr, header->size);
    ufree(ptr);
    return new_block;
}

void umemstats(void){
    size_t allocated_memory = 0;
    size_t free_memory = 0;
    size_t fragmentation = calculateFragmentation();

    printumemstats((int)total_allocations, 
                   (int)totalDeallocations, 
                   (long)allocated_memory, 
                   (long)free_memory, 
                   (double)fragmentation);
}

void *best_fit(size_t size) {
    node_t *best_fit = NULL;
    node_t *current = freeList;

    while (current) {
        if (current->size >= size) {
            if (!best_fit || current->size < best_fit->size) {
                best_fit = current;
            }
        }
        current = current->next;
    }

    return (best_fit) ? (void *)((char *)best_fit + sizeof(header_t)) : NULL;
}

void *worst_fit(size_t size) {
    node_t *worst_fit = NULL;
    node_t *current = freeList;

    while (current) {
        if (current->size >= size) {
            if (!worst_fit || current->size > worst_fit->size) {
                worst_fit = current;
            }
        }
        current = current->next;
    }

    return (worst_fit) ? (void *)((char *)worst_fit + sizeof(header_t)) : NULL;
}

void *first_fit(size_t size) {
    node_t *current = freeList;

    while (current) {
        if (current->size >= size) {
            return (void *)((char *)current + sizeof(header_t));
        }
        current = current->next;
    }

    return NULL;
}

void *next_fit(size_t size) {
    node_t *current = (lastAlloc) ? lastAlloc->next : freeList;

    while (current) {
        if (current->size >= size) {
            lastAlloc = current;
            return (void *)((char *)current + sizeof(header_t));
        }
        current = current->next;
    }

    current = freeList;
    while (current && current != lastAlloc) {
        if (current->size >= size) {
            lastAlloc = current;
            return (void *)((char *)current + sizeof(header_t));
        }
        current = current->next;
    }

    return NULL;
}

// Function to mark the block as allocated in memory
void blockAllocated(void *block, size_t size) {
    header_t *header = (header_t *)block;
    header->magic = MAGIC;
    header->size = size;

    // Calculate potential split if there is extra space
    node_t *new_block = (node_t *)((char *)block + sizeof(header_t) + size);
    if (freeList->size >= size + sizeof(header_t) + 8) {
        new_block->size = ((node_t *)header)->size - size - sizeof(header_t);
        new_block->next = ((node_t *)header)->next;
        ((node_t *)header)->next = new_block;
    }
}

// Function to coalesce free blocks
void coalesce(){
    node_t *current = freeList;

    while (current && current->next) {
        node_t *next = current->next;

        if ((char *)current + sizeof(header_t) + current->size == (char *)next) {
            current->size += sizeof(header_t) + next->size;
            current->next = next->next;
        } else {
            current = current->next;
        }
    }
}

void addToFreeList(node_t *header) {
    node_t *current = freeList;

    // Check if the block is already in the free list (double-free detection)
    while (current) {
        if (current == header) {
            fprintf(stderr, "Error: Double-free detected at block %p\n", (void *)header);
            exit(1);
        }
        current = current->next;
    }

    // If not in the free list, add it
    node_t *prev = NULL;
    current = freeList;
    while (current && current < header) {
        prev = current;
        current = current->next;
    }

    header->next = current;
    if (prev) {
        prev->next = header;
    } else {
        freeList = header;
    }
}

size_t calculateFragmentation(){
    size_t totalFree = 0;
    size_t fragmentedFree = 0;
    node_t *current = freeList;

    while (current) {
        totalFree += current->size;
        if (current->size < totalFree / 2) {
            fragmentedFree += current->size;
        }
        current = current->next;
    }

    return (totalFree == 0) ? 0 : (fragmentedFree * 100) / totalFree;
}
