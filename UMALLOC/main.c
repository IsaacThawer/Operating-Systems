#include "umem.h"
#include <stdio.h>

int main() {
    // Test 1: Initialize memory allocator with a valid size and BEST_FIT algorithm
    printf("Test 1: Initializing memory allocator\n");
    if (umeminit(4096, BEST_FIT) == 0) {
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }
    

    // Test 2: Allocate a memory block and check if it succeeds
    printf("Test 2: Allocating a memory block of 16 bytes\n");
    void *ptr1 = umalloc(16);
    if (ptr1 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr1);
        
    } else {
        printf("Allocation failed.\n");
    }
	umemstats();
	
    // Test 3: Allocate another memory block and check if it succeeds
    printf("Test 3: Allocating a memory block of 32 bytes\n");
    void *ptr2 = umalloc(32);
    if (ptr2 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr2);
    } else {
        printf("Allocation failed.\n");
    }
    	umemstats();

    // Test 4: Free the first memory block
    printf("Test 4: Freeing the first memory block\n");
    if (ufree(ptr1) == 1) {
        printf("Free successful.\n");
    } else {
        printf("Free failed.\n");
    }

    // Test 5: Attempt double-free on the same block (should detect and handle it)
    printf("Test 5: Attempting double-free on the first memory block\n");
    if (ufree(ptr1) == 1) {
        printf("Double-free detected and handled.\n");
    }

    // Test 6: Reallocate a block to a larger size
    printf("Test 6: Reallocating the second block to 64 bytes\n");
    void *ptr3 = urealloc(ptr2, 64);
    if (ptr3 != NULL) {
        printf("Reallocation successful. New pointer: %p\n", ptr3);
    } else {
        printf("Reallocation failed.\n");
    }

    // Test 7: Free the reallocated block
    printf("Test 7: Freeing the reallocated block\n");
    if (ufree(ptr3) == 1) {
        printf("Free successful.\n");
    } else {
        printf("Free failed.\n");
    }

    // Test 8: Allocation after multiple operations
    printf("Test 8: Allocating a large memory block of 128 bytes\n");
    void *ptr4 = umalloc(128);
    if (ptr4 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr4);
    } else {
        printf("Allocation failed.\n");
    }

    // Test 9: Print memory statistics
    printf("Test 9: Printing memory statistics\n");
    umemstats();

    // Clean up and exit
    if (ptr4 != NULL) ufree(ptr4);

    printf("All tests completed.\n");
    return 0;
}
