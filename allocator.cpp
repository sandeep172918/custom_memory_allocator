#include "allocator.h"
#include <cassert>
#include <cstdint> // for std::uintptr_t
#include <iostream>
#include<string>

// Constructor: Allocates the memory buffer and initializes the free list.
FreeListAllocator::FreeListAllocator(std::size_t size, AllocationStrategy strategy) 
    : m_size(size), m_strategy(strategy) {
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

    switch (m_strategy) {
        case AllocationStrategy::FirstFit: {
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
                        // Use the entire block
                        if (prev_node) {
                            prev_node->next = current_node->next;
                        } else {
                            m_free_list_head = current_node->next;
                        }
                    }

                    std::uintptr_t allocated_address = current_address + adjustment;
                    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(allocated_address);
                    header->size = total_required_size;
                    header->adjustment = adjustment;

                    return reinterpret_cast<void*>(allocated_address + header_size);
                }

                prev_node = current_node;
                current_node = current_node->next;
            }
            break; 
        }
        case AllocationStrategy::BestFit: {
            FreeNode* best_node = nullptr;
            FreeNode* best_node_prev = nullptr;
            FreeNode* prev_node = nullptr;
            FreeNode* current_node = m_free_list_head;
            std::size_t best_total_required_size = 0;

            while (current_node != nullptr) {
                const std::uintptr_t current_address = reinterpret_cast<std::uintptr_t>(current_node);
                const std::size_t header_size = sizeof(AllocationHeader);
                std::size_t adjustment = 0;
                std::uintptr_t data_address = current_address + header_size;
                std::size_t remainder = data_address % alignment;
                if (remainder != 0) {
                    adjustment = alignment - remainder;
                }
                const std::size_t total_required_size = size + header_size + adjustment;

                if (current_node->size >= total_required_size) {
                    if (best_node == nullptr || current_node->size < best_node->size) {
                        best_node = current_node;
                        best_node_prev = prev_node;
                        best_total_required_size = total_required_size;
                    }
                }
                prev_node = current_node;
                current_node = current_node->next;
            }

            if (best_node != nullptr) {
                const std::uintptr_t current_address = reinterpret_cast<std::uintptr_t>(best_node);
                const std::size_t header_size = sizeof(AllocationHeader);
                std::size_t adjustment = 0;
                std::uintptr_t data_address = current_address + header_size;
                std::size_t remainder = data_address % alignment;
                if (remainder != 0) {
                    adjustment = alignment - remainder;
                }

                std::size_t remaining_size = best_node->size - best_total_required_size;
                if (remaining_size > sizeof(FreeNode)) {
                    FreeNode* new_node = reinterpret_cast<FreeNode*>(current_address + best_total_required_size);
                    new_node->size = remaining_size;
                    new_node->next = best_node->next;
                    if (best_node_prev) {
                        best_node_prev->next = new_node;
                    } else {
                        m_free_list_head = new_node;
                    }
                } else {
                    if (best_node_prev) {
                        best_node_prev->next = best_node->next;
                    } else {
                        m_free_list_head = best_node->next;
                    }
                }

                std::uintptr_t allocated_address = current_address + adjustment;
                AllocationHeader* header = reinterpret_cast<AllocationHeader*>(allocated_address);
                header->size = best_total_required_size;
                header->adjustment = adjustment;

                return reinterpret_cast<void*>(allocated_address + header_size);
            }
            break;
        }
    }

    return nullptr;
}

// Deallocate: Frees a block of memory and performs coalescing.
void FreeListAllocator::deallocate(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(static_cast<char*>(ptr) - sizeof(AllocationHeader));
    std::uintptr_t block_start = reinterpret_cast<std::uintptr_t>(ptr) - sizeof(AllocationHeader) - header->adjustment;
    std::size_t block_size = header->size;

    FreeNode* new_free_node = reinterpret_cast<FreeNode*>(block_start);
    new_free_node->size = block_size;

    FreeNode* prev_node = nullptr;
    FreeNode* current_node = m_free_list_head;

    while (current_node != nullptr && current_node < new_free_node) {
        prev_node = current_node;
        current_node = current_node->next;
    }

    if (prev_node == nullptr) {
        new_free_node->next = m_free_list_head;
        m_free_list_head = new_free_node;
    } else {
        new_free_node->next = prev_node->next;
        prev_node->next = new_free_node;
    }
    
    if (new_free_node->next != nullptr && 
        (reinterpret_cast<std::uintptr_t>(new_free_node) + new_free_node->size) == reinterpret_cast<std::uintptr_t>(new_free_node->next)) {
        new_free_node->size += new_free_node->next->size;
        new_free_node->next = new_free_node->next->next;
    }
    
    if (prev_node != nullptr && 
        (reinterpret_cast<std::uintptr_t>(prev_node) + prev_node->size) == reinterpret_cast<std::uintptr_t>(new_free_node)) {
        prev_node->size += new_free_node->size;
        prev_node->next = new_free_node->next;
    }
}

void FreeListAllocator::printStatus() const {
    std::cout << "--- Allocator Status ---\n";
    std::cout << "Free List:\n";
    FreeNode* current = m_free_list_head;
    if (current == nullptr) {
        std::cout << "  (empty)\n";
    }
    while (current != nullptr) {
        std::cout << "  - Block at " << static_cast<void*>(current) 
                  << " | Size: " << current->size << " bytes\n";
        current = current->next;
    }
    std::cout << "------------------------\n";
}

std::size_t FreeListAllocator::getAllocationSize(void* ptr) const {
    if (ptr == nullptr) {
        return 0;
    }
    AllocationHeader* header_at_data = reinterpret_cast<AllocationHeader*>(static_cast<char*>(ptr) - sizeof(AllocationHeader));
    return header_at_data->size;
}
