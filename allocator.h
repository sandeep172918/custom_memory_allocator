#ifndef FREE_LIST_ALLOCATOR_H
#define FREE_LIST_ALLOCATOR_H

#include <cstddef> // for std::size_t

class FreeListAllocator {
public:
    enum class AllocationStrategy {
        FirstFit,
        BestFit
    };

    FreeListAllocator(std::size_t size, AllocationStrategy strategy = AllocationStrategy::FirstFit);
    ~FreeListAllocator();

    void* allocate(std::size_t size, std::size_t alignment = alignof(void*));
    void deallocate(void* ptr);
    void reset();
    void printStatus() const;
    std::size_t getAllocationSize(void* ptr) const;

private:
    // Struct to hold allocation metadata. Placed just before the user's data block.
    struct AllocationHeader {
        std::size_t size;      // Size of the data block
        std::size_t adjustment; // Adjustment for alignment
    };

    // A node in our linked list of free memory blocks
    struct FreeNode {
        std::size_t size;    // Size of the free block
        FreeNode* next;      // Pointer to the next free block
    };

    FreeListAllocator(const FreeListAllocator&) = delete;
    FreeListAllocator& operator=(const FreeListAllocator&) = delete;

    void* m_start = nullptr;
    std::size_t m_size;
    AllocationStrategy m_strategy;
    
    FreeNode* m_free_list_head = nullptr;
};

#endif // FREE_LIST_ALLOCATOR_H