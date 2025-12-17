#include "allocator.h"
#include <iostream>
#include <vector>

// Helper function to print a message and pointer address
void print_op(const std::string& msg, void* ptr) {
    std::cout << msg << "\t-> " << ptr << std::endl;
}

int main() {
    std::cout << "--- Free-List Memory Allocator Demo ---" << std::endl;

    // 1. Create a FreeListAllocator with a 1 KiB buffer.
    const std::size_t buffer_size = 1024;
    FreeListAllocator allocator(buffer_size);
    std::cout << "Allocator created with " << buffer_size << " bytes." << std::endl;

    // 2. Initial allocation pass
    std::cout << "\n--- Initial Allocation Pass ---" << std::endl;
    void* block1 = allocator.allocate(128, 8);
    print_op("Allocated block1 (128 bytes)", block1);
    
    void* block2 = allocator.allocate(64, 8);
    print_op("Allocated block2 (64 bytes)", block2);

    void* block3 = allocator.allocate(256, 8);
    print_op("Allocated block3 (256 bytes)", block3);

    // 3. Deallocate a block from the middle
    std::cout << "\n--- Deallocation and Reuse ---" << std::endl;
    print_op("Deallocating block2 (64 bytes)", block2);
    allocator.deallocate(block2);

    // 4. Allocate a smaller block to show reuse
    // This should reuse the memory from block2
    void* block4_reused = allocator.allocate(32, 8);
    print_op("Allocated block4 (32 bytes)", block4_reused);
    std::cout << "(Note: Address should be same or similar to freed block2)" << std::endl;

    // 5. Test Coalescing
    std::cout << "\n--- Testing Coalescing ---" << std::endl;
    void* block5 = allocator.allocate(100, 8);
    print_op("Allocated block5 (100 bytes)", block5);

    // Deallocate adjacent blocks (block1 and the remainder of block2's original space)
    // Note: The original block2 is now split between block4 and a new free block.
    // To test coalescing, we free block1, then block4.
    print_op("Deallocating block1 (128 bytes)", block1);
    allocator.deallocate(block1);
    
    print_op("Deallocating block4 (32 bytes)", block4_reused);
    allocator.deallocate(block4_reused);
    std::cout << "(block1 and the space from block4 are now free and should be coalesced)" << std::endl;

    // 6. Allocate a block that can only fit if coalescing worked
    // The coalesced block should be at least 128 + 64 bytes (original block1 + block2 size)
    // minus the allocation header sizes.
    std::cout << "\n--- Testing Reuse of Coalesced Memory ---" << std::endl;
    void* large_block = allocator.allocate(150, 8);
    print_op("Allocated large_block (150 bytes)", large_block);
    if (large_block) {
        std::cout << "SUCCESS: Large allocation succeeded, so coalescing worked!" << std::endl;
    } else {
        std::cout << "FAILURE: Large allocation failed, coalescing might not have worked." << std::endl;
    }

    std::cout << "\n--- Demo Complete ---" << std::endl;

    return 0;
}