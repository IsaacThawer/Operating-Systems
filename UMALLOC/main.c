#include "umem.h"
#include <stdio.h>

int testFragmentation();
int testFunctions();
int testInit();
int doubleFree();
int testNextFit();
int testWorstFit();
int testFirstFit();
int testEmpty();
int testStress();

int main(){
//testFragmentation();
//testFunctions();
//testInit();
//doubleFree();
//testNextFit();
//testWorstFit();
//testFirstFit();
//testEmpty();
testStress();
}



int testFunctions() {
    
    // Test 1: Initialize memory and test to see if all functions work
    printf("Test 1: Initializing memory allocator\n");
    if (umeminit(4096, BEST_FIT) == 0) { // size of heap is 4096 and it is using the best fit algorithm
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }
        printf("\n");
    // Test 2: Allocate a memory block and check if it succeeds
    
    printf("Test 2: Allocating a memory block of 1 bytes\n");
    void *ptr1 = umalloc(1);
    if (ptr1 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr1);
        
    } else {
        printf("Allocation failed.\n");
    }
	    umemstats();
        printf("\n");
    
    // Test 3: Allocate another memory block and check if it succeeds
    printf("Test 3: Allocating a memory block of 32 bytes\n");
    void *ptr2 = umalloc(32);
    if (ptr2 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr2);
    } else {
        printf("Allocation failed.\n");
    }
    	umemstats();
        printf("\n");

    // Test 4: Free the first memory block
    printf("Test 4: Freeing the first memory block\n");
    if (ufree(ptr1) == 1) {
        printf("Free successful.\n");
    } else {
        printf("Free failed.\n");
    }
        umemstats();
        printf("\n");

     // Test 5: Allocate another memory block and check if it succeeds
    printf("Test 5: Allocating a memory block of 64 bytes\n");
    void *ptr5 = umalloc(64);
    if (ptr5 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr5);
    } else {
        printf("Allocation failed.\n");
    }
    	umemstats();
        printf("\n");

    // Test 6: Reallocate a block to a larger size
    printf("Test 6: Reallocating the second block to 64 bytes\n");
    void *ptr3 = urealloc(ptr2, 64);
    if (ptr3 != NULL) {
        printf("Reallocation successful. New pointer: %p\n", ptr3);
    } else {
        printf("Reallocation failed.\n");
    }

        umemstats();
        printf("\n");

    // Test 7: Free the reallocated block
    printf("Test 7: Freeing the reallocated block\n");
    if (ufree(ptr3) == 1) {
        printf("Free successful.\n");
    } else {
        printf("Free failed.\n");
    }
        printf("\n");

    // Test 8: Allocation after multiple operations
    printf("Test 8: Allocating a large memory block of 128 bytes\n");
    void *ptr4 = umalloc(128);
    if (ptr4 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr4);
    } else {
        printf("Allocation failed.\n");
    }
        printf("\n");

    // Test 9: Print memory statistics
    printf("Test 9: Printing memory statistics\n");
    umemstats();

    // Clean up and exit
    if (ptr4 != NULL) ufree(ptr4);

    printf("All tests completed.\n");
    return 0;
   
}


int testFragmentation() { // this tests for fragmentations and the calculations regarding them with BEST_FIT
    // Test 1: Initialize memory allocator with a valid size and BEST_FIT algorithm
    printf("Test 1: Initializing memory allocator\n");
    if (umeminit(1024, BEST_FIT) == 0) { // Initializing a heap of 1024 bytes with BEST_FIT
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }
        printf("\n");

    // Test 2: Allocate initial blocks to create some fragmentation
    printf("Test 2: Allocating blocks to create fragmentation\n");
    void *ptr1 = umalloc(100);  
    void *ptr2 = umalloc(200);  
    void *ptr3 = umalloc(50);   
    void *ptr4 = umalloc(150);  
    if (ptr1 && ptr2 && ptr3 && ptr4) {
        printf("Initial allocations successful.\n");
    } else {
        printf("Initial allocation failed.\n");
        return 1;
    }
        printf("\n");

    // Test 3: Free some blocks to create fragmented free list
    printf("Test 3: Freeing blocks to create fragmentation\n");
    ufree(ptr1);  // Free 100-byte block
    ufree(ptr3);  // Free 50-byte block
        umemstats();
        printf("\n");

    // Test 4: Allocate a block that should use BEST_FIT to choose the 50-byte free block
    printf("Test 4: Allocating a 40-byte block (BEST_FIT should pick the 50-byte free block)\n");
    void *ptr5 = umalloc(40);
    if (ptr5 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr5);
    } else {
        printf("Allocation failed.\n");
    }
        umemstats();
        printf("\n");

    // Test 5: Allocate another block that requires BEST_FIT to pick the 100-byte free block
    printf("Test 5: Allocating a 90-byte block (BEST_FIT should pick the 100-byte free block)\n");
    void *ptr6 = umalloc(90);
    if (ptr6 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr6);
    } else {
        printf("Allocation failed.\n");
    }
        umemstats();
        printf("\n");

    // Test 6: Clean up remaining allocations
    printf("Test 6: Cleaning up allocations\n");
    ufree(ptr2);
    ufree(ptr4);
    ufree(ptr5);
    ufree(ptr6);
        umemstats();
        printf("\n");

    printf("All tests completed.\n");
    return 0;
}

int testInit() {
    
    
    printf("Test 1: Initializing memory allocator\n");
    if (umeminit(4096, BEST_FIT) == 0) { 
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }

    printf("\n");

    // Test 1: Initialize the memory again to see if the flag works properly
    printf("Test 2: Attempting to re-initialize memory allocator\n");
    if (umeminit(8192, WORST_FIT) == 0) {
        printf("Initialization successful.\n");
    } else {
        printf("Already initialized memory\n");
    }

    return 0; 
}

int doubleFree(){
    
    printf("Initializing memory allocator\n");
    if (umeminit(4096, BEST_FIT) == 0) { // size of heap is 4096 and it is using the best fit algorithm
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }
        printf("\n");

    // Test 1: Allocate another memory block and check if it succeeds
    printf("Test 1: Allocating a memory block of 64 bytes\n");
    void *ptr5 = umalloc(64);
    if (ptr5 != NULL) {
        printf("Allocation successful. Pointer: %p\n", ptr5);
    } else {
        printf("Allocation failed.\n");
    }
        
    	umemstats();
        printf("\n");

     // Test 2: Free the first memory block
    printf("Test 2: Freeing the first memory block\n");
    if (ufree(ptr5) == 1) {
        printf("Free successful.\n");
    } else {
        printf("Free failed.\n");
    }
        printf("\n");

    // Test 3: Attempt double-free on the same block (should detect and handle it)
    printf("Test 3: Attempting double-free on the first memory block\n");
    if (ufree(ptr5) == 1) {
        printf("Double-free detected and not handled.\n");
    }
        printf("\n");
    return 0;
}

int testNextFit() {
    printf("Initializing memory allocator with NEXT_FIT algorithm\n");
    if (umeminit(1024, NEXT_FIT) == 0) { // Initialize with 1024 bytes
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }

    // Allocate multiple blocks to see the "next fit" behavior
    printf("Allocating 100 bytes\n");
    void *ptr1 = umalloc(100);
    printf("Allocating 200 bytes\n");
    void *ptr2 = umalloc(200);
    printf("Allocating 50 bytes\n");
    void *ptr3 = umalloc(50);

    printf("\n");
    umemstats();
    printf("\n");

    // Free first block to create space for next fit
    printf("Freeing 100-byte block\n");
    ufree(ptr1);

    printf("\n");
    umemstats();
    printf("\n");

    // Allocate a block that should use the space freed by ptr1
    printf("Allocating 90 bytes (Next Fit should select the free 100-byte block)\n");
    void *ptr4 = umalloc(90);

    if (ptr4) {
        printf("Allocation successful.\n");
    } else {
        printf("Allocation failed.\n");
    }

    printf("\n");
    umemstats();

    // Clean up
    ufree(ptr2);
    ufree(ptr3);
    ufree(ptr4);

    return 0;
}
int testWorstFit() {
    printf("Initializing memory allocator with WORST_FIT algorithm\n");
    if (umeminit(1024, WORST_FIT) == 0) { // Initialize with 1024 bytes
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }

    // Allocate multiple blocks and then free some to create large gaps
    printf("Allocating 100 bytes\n");
    void *ptr1 = umalloc(100);
    printf("Allocating 200 bytes\n");
    void *ptr2 = umalloc(200);
    printf("Allocating 50 bytes\n");
    void *ptr3 = umalloc(50);

    printf("Freeing 200-byte block\n");
    ufree(ptr2);

    // Now, we have a large free block of 200 bytes
    printf("Allocating 80 bytes (Worst Fit should select the 200-byte block)\n");
    void *ptr4 = umalloc(80);

    if (ptr4) {
        printf("Allocation successful.\n");
    } else {
        printf("Allocation failed.\n");
    }
    umemstats();

    // Clean up
    ufree(ptr1);
    ufree(ptr3);
    ufree(ptr4);

    return 0;
}
int testFirstFit() {
    printf("Initializing memory allocator with FIRST_FIT algorithm\n");
    if (umeminit(1024, FIRST_FIT) == 0) { // Initialize with 1024 bytes
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }

    // Allocate blocks and then free some to create gaps
    printf("Allocating 100 bytes\n");
    void *ptr1 = umalloc(100);
    printf("Allocating 50 bytes\n");
    void *ptr2 = umalloc(50);
    printf("Allocating 200 bytes\n");
    void *ptr3 = umalloc(200);

    printf("Freeing 100-byte block\n");
    ufree(ptr1);

    // Attempt to allocate 80 bytes, which should fit into the first free block (100 bytes)
    printf("Allocating 80 bytes (First Fit should select the 100-byte block)\n");
    void *ptr4 = umalloc(80);

    if (ptr4) {
        printf("Allocation successful.\n");
    } else {
        printf("Allocation failed.\n");
    }
    umemstats();

    // Clean up
    ufree(ptr2);
    ufree(ptr3);
    ufree(ptr4);

    return 0;
}

int testEmpty(){

    printf("Initializing memory allocator with WORST_FIT algorithm, but 0 Bytes\n");
    if (umeminit(0, WORST_FIT) == 0) { // Initialize with 1024 bytes
        printf("Initialization successful.\n");
    } else {
        printf("Initialization failed.\n");
        return 1;
    }
    return 0;

}
int testStress() {
    printf("Stress Testing with Many Small Allocations\n");
    umeminit(4096, FIRST_FIT);

    void *pointers[50];
    for (int i = 0; i < 50; i++) {
        pointers[i] = umalloc(10);  // Allocate 50 small blocks
    }
    umemstats();

    printf("\n");
    // Free all blocks in reverse order to check stability
    for (int i = 49; i >= 0; i--) {
        ufree(pointers[i]);
    }
    umemstats();
    return 0;
}

