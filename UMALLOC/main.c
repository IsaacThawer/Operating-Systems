#include "umem.h"      // Include allocator definitions
#include <stdio.h>
#include <assert.h>    // For assertions in tests

void test_umeminit();
void test_umalloc_best_fit();
void test_umalloc_worst_fit();
void test_ufree();
void test_urealloc();
void test_umemstats();

int main() {
    printf("Starting memory allocator tests...\n");

    test_umeminit();
    test_umalloc_best_fit();
    test_umalloc_worst_fit();
    test_ufree();
    test_urealloc();
    test_umemstats();

    printf("All tests passed!\n");
    return 0;
}

void test_umeminit() {
    printf("Running test_umeminit...\n");
    
    assert(umeminit(1024 * 1024, BEST_FIT) == 0);  // Initialize 1MB with BEST_FIT
    assert(umeminit(1024 * 1024, WORST_FIT) == -1);  // Should fail, already initialized
    
    printf("test_umeminit passed!\n");
}

void test_umalloc_best_fit() {
    printf("Running test_umalloc_best_fit...\n");

    umeminit(1024 * 1024, BEST_FIT);  // Initialize with BEST_FIT
    void *ptr1 = umalloc(100);
    void *ptr2 = umalloc(200);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr1 != ptr2);  // Ensure different allocations

    ufree(ptr1);
    ufree(ptr2);

    printf("test_umalloc_best_fit passed!\n");
}

void test_umalloc_worst_fit() {
    printf("Running test_umalloc_worst_fit...\n");

    umeminit(1024 * 1024, WORST_FIT);  // Initialize with WORST_FIT
    void *ptr1 = umalloc(100);
    void *ptr2 = umalloc(200);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr1 != ptr2);  // Ensure different allocations

    ufree(ptr1);
    ufree(ptr2);

    printf("test_umalloc_worst_fit passed!\n");
}

void test_ufree() {
    printf("Running test_ufree...\n");

    umeminit(1024 * 1024, FIRST_FIT);  // Initialize with FIRST_FIT
    void *ptr = umalloc(100);
    assert(ptr != NULL);
    ufree(ptr);

    // Trying to double-free should trigger an error
    fprintf(stderr, "Expecting memory corruption error message...\n");
    ufree(ptr);  // This should raise an error and exit if memory corruption checks are implemented correctly

    printf("test_ufree passed!\n");
}

void test_urealloc() {
    printf("Running test_urealloc...\n");

    umeminit(1024 * 1024, NEXT_FIT);  // Initialize with NEXT_FIT
    void *ptr = umalloc(100);
    assert(ptr != NULL);

    // Reallocate with larger size
    ptr = urealloc(ptr, 200);
    assert(ptr != NULL);

    // Reallocate with smaller size
    ptr = urealloc(ptr, 50);
    assert(ptr != NULL);

    ufree(ptr);

    printf("test_urealloc passed!\n");
}

void test_umemstats() {
    printf("Running test_umemstats...\n");

    umeminit(1024 * 1024, BEST_FIT);  // Initialize with BEST_FIT
    void *ptr1 = umalloc(100);
    void *ptr2 = umalloc(200);

    ufree(ptr1);
    ufree(ptr2);

    umemstats();  // Should print statistics

    printf("test_umemstats passed!\n");
}
