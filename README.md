# Custom C++ Free-List Memory Allocator

This project is a mini-demonstration of a custom memory allocator in C++. It implements a **Free-List Allocator** which supports individual allocations, deallocations, and memory coalescing to reduce fragmentation.

## What is a Free-List Allocator?

A Free-List Allocator manages a collection of free memory blocks (the "free list"). Unlike a simple Linear Allocator, it allows for individual blocks of memory to be freed and reused.

### Core Concepts

1.  **Allocation:**
    -   When a memory request is made, the allocator searches its linked list of free blocks for one that is large enough (this implementation uses a "First-Fit" strategy).
    -   If the found block is much larger than required, it is split. The requested amount is returned to the user, and the remainder is kept as a smaller free block in the list.
    -   A small header containing metadata (like the size of the allocation) is stored just before the memory block returned to the user.
    - All allocations are aligned to 8 bytes to ensure safe access for common data types.


2.  **Deallocation:**
    -   When a pointer is deallocated, the allocator retrieves its metadata header.
    -   The freed block is then added back to the free list. To make coalescing efficient, the list is kept sorted by memory address.

3.  **Coalescing:**
    -   This is the key to combating fragmentation. When a block is freed, the allocator checks if the memory blocks immediately before or after it are also free.
    -   If they are, the adjacent free blocks are merged (coalesced) into a single, larger free block. This helps to ensure that large contiguous blocks of memory remain available for future allocations.

## Project Structure

```
.
├── allocator.h         # Header file for the FreeListAllocator class.
├── allocator.cpp       # Implementation of the FreeListAllocator.
├── main.cpp            # A demonstration of how to use the allocator.
├── CMakeLists.txt      # Build script for CMake.
└── README.md           # This file.
```

## How to Build and Run

To build and run this project, you will need a C++ compiler and CMake.

### Prerequisites

-   A C++ compiler (like GCC, Clang, or MSVC)
-   [CMake](https://cmake.org/download/) (version 3.10 or higher)

### Build Steps

1.  **Clone the Repository (or download the files).**

2.  **Create a Build Directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Run CMake to Configure the Build:**
    ```bash
    cmake ..
    ```

4.  **Compile the Project:**
    ```bash
    cmake --build .
    ```

5.  **Run the Executable:**
    The executable `allocator_demo` will be created in the `build` directory. Run it to see the output.
    ```bash
    ./allocator_demo
    ```

### Expected Output

You should see an output demonstrating the allocator allocating blocks, deallocating a block, reusing the freed memory for a new allocation, and finally, coalescing adjacent free blocks to satisfy a larger allocation request.

```
--- Free-List Memory Allocator Demo ---
Allocator created with 1024 bytes.

--- Initial Allocation Pass ---
Allocated block1 (128 bytes)    -> 0x...
Allocated block2 (64 bytes)     -> 0x...
Allocated block3 (256 bytes)    -> 0x...

--- Deallocation and Reuse ---
Deallocating block2 (64 bytes)  -> 0x...
Allocated block4 (32 bytes)     -> 0x...
(Note: Address should be same or similar to freed block2)

--- Testing Coalescing ---
Allocated block5 (100 bytes)    -> 0x...
Deallocating block1 (128 bytes) -> 0x...
Deallocating block4 (32 bytes)  -> 0x...
(block1 and the space from block4 are now free and should be coalesced)

--- Testing Reuse of Coalesced Memory ---
Allocated large_block (150 bytes)       -> 0x...
SUCCESS: Large allocation succeeded, so coalescing worked!

--- Demo Complete ---
```
## Limitations and Future Work

- Uses a first-fit allocation strategy; alternative strategies such as best-fit
  or segregated free lists could further reduce fragmentation.
- The allocator is not thread-safe and would require synchronization mechanisms
  (e.g., mutexes) for concurrent use.
- Memory size is fixed at initialization and does not grow dynamically.
