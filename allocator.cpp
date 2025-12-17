#include "allocator.h"
#include <cassert>
#include <cstdint> // for std::uintptr_t

// Constructor: Allocates the memory buffer and initializes the free list.
FreeListAllocator::FreeListAllocator(std::size_t size) : m_size(size) {
    if (size > 0) {
        m_start = new char[size];
        reset();
    }
}

// Destructor: Frees the memory buffer.
FreeListAllocator::~FreeListAllocator() {
    delete[] static_cast<char*>(m_start);
}

// Reset: Resets the allocator to have a single free block of the total size.
void FreeListAllocator::reset() {
    m_free_list_head = static_cast<FreeNode*>(m_start);
    m_free_list_head->size = m_size;
    m_free_list_head->next = nullptr;
}

// Allocate: Finds a suitable free block and returns a pointer to the user.
void* FreeListAllocator::allocate(std::size_t size, std::size_t alignment) {
    assert(size > 0 && "Allocation size must be greater than 0.");
    assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "Alignment must be a power of 2.");

    FreeNode* prev_node = nullptr;
    FreeNode* current_node = m_free_list_head;

    while (current_node != nullptr) {
        // Calculate adjustment needed for alignment
        const std::uintptr_t current_address = reinterpret_cast<std::uintptr_t>(current_node);
        const std::size_t header_size = sizeof(AllocationHeader);
        std::size_t adjustment = 0;
        
        std::uintptr_t data_address = current_address + header_size;
        std::size_t remainder = data_address % alignment;
        if (remainder != 0) {
            adjustment = alignment - remainder;
        }

        const std::size_t total_required_size = size + header_size + adjustment;

        // If the current free block is large enough
        if (current_node->size >= total_required_size) {
            // Check if the remaining space is large enough to create a new FreeNode
            std::size_t remaining_size = current_node->size - total_required_size;
            if (remaining_size > sizeof(FreeNode)) {
                // Split the block
                FreeNode* new_node = reinterpret_cast<FreeNode*>(current_address + total_required_size);
                new_node->size = remaining_size;
                new_node->next = current_node->next;

                if (prev_node) {
                    prev_node->next = new_node;
                } else {
                    m_free_list_head = new_node;
                }
            } else {
                // Use the entire block, no splitting
                if (prev_node) {
                    prev_node->next = current_node->next;
                } else {
                    m_free_list_head = current_node->next;
                }
            }

            // Set up the allocation header
            std::uintptr_t allocated_address = current_address + adjustment;
            AllocationHeader* header = reinterpret_cast<AllocationHeader*>(allocated_address);
            header->size = total_required_size;
            header->adjustment = adjustment;

            return reinterpret_cast<void*>(allocated_address + header_size);
        }

        // Move to the next free block
        prev_node = current_node;
        current_node = current_node->next;
    }

    // Out of memory
    return nullptr;
}

// Deallocate: Frees a block of memory and performs coalescing.
void FreeListAllocator::deallocate(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    // Get the allocation header
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(static_cast<char*>(ptr) - sizeof(AllocationHeader));
    std::uintptr_t block_start = reinterpret_cast<std::uintptr_t>(ptr) - sizeof(AllocationHeader) - header->adjustment;
    std::size_t block_size = header->size;

    FreeNode* new_free_node = reinterpret_cast<FreeNode*>(block_start);
    new_free_node->size = block_size;

    // Insert the new free node into the sorted list and coalesce
    FreeNode* prev_node = nullptr;
    FreeNode* current_node = m_free_list_head;

    // Find the correct position to insert the new node to keep the list sorted by address
    while (current_node != nullptr && current_node < new_free_node) {
        prev_node = current_node;
        current_node = current_node->next;
    }

    if (prev_node == nullptr) {
        // Insert at the head of the list
        new_free_node->next = m_free_list_head;
        m_free_list_head = new_free_node;
    } else {
        new_free_node->next = prev_node->next;
        prev_node->next = new_free_node;
    }
    
    // Coalesce with the next block if possible
    if (new_free_node->next != nullptr && 
        (reinterpret_cast<std::uintptr_t>(new_free_node) + new_free_node->size) == reinterpret_cast<std::uintptr_t>(new_free_node->next)) {
        new_free_node->size += new_free_node->next->size;
        new_free_node->next = new_free_node->next->next;
    }
    
    // Coalesce with the previous block if possible
    if (prev_node != nullptr && 
        (reinterpret_cast<std::uintptr_t>(prev_node) + prev_node->size) == reinterpret_cast<std::uintptr_t>(new_free_node)) {
        prev_node->size += new_free_node->size;
        prev_node->next = new_free_node->next;
    }
}