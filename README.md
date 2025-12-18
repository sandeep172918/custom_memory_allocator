# Custom C++ Free-List Memory Allocator

This project is a C++ implementation of a custom memory allocator. It uses a free-list approach to manage a fixed-size buffer of memory, with support for different allocation strategies. The project includes an interactive shell to demonstrate how the allocator works.

## About the Project

A free-list allocator is a memory management technique that keeps track of available memory blocks in a linked list. When a request for memory is made, the allocator searches the free list for a suitable block. If a block is found, it may be split to fit the requested size, and the remaining portion is kept on the free list. When memory is deallocated, it is returned to the free list, and adjacent free blocks are merged (coalesced) to reduce fragmentation.

This implementation provides a `FreeListAllocator` class that can be configured to use one of two allocation strategies:

*   **First Fit:** The allocator chooses the first available block in the free list that is large enough to satisfy the request.
*   **Best Fit:** The allocator searches the entire free list to find the smallest block that is large enough to satisfy the request.

## Features

*   **Custom Memory Management:** Manages a fixed-size memory buffer.
*   **Two Allocation Strategies:** Supports both "First Fit" and "Best Fit" allocation strategies.
*   **Memory Coalescing:** Merges adjacent free blocks to reduce fragmentation.
*   **Alignment Support:** Allocates memory with a specified alignment.
*   **Interactive Shell:** A command-line interface to test and demonstrate the allocator's functionality.

## Getting Started

To build and run this project, you will need a C++ compiler (like GCC, Clang, or MSVC) and CMake (version 3.10 or higher).

### Build Instructions

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/sandeep172918/custom_memory_allocator.git
    cd custom-memory-allocator
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the build with CMake:**
    ```bash
    cmake ..
    ```

4.  **Compile the project:**
    ```bash
    cmake --build .
    ```

### Running the Demo

The build process will create an executable named `freelist_demo` in the `build` directory. Run it to start the interactive shell:

```bash
./freelist_demo
```

## Usage

The interactive shell allows you to test the memory allocator with the following commands:

*   `alloc <size> [name]`: Allocates a block of memory of the specified size. You can optionally assign a name to the allocation for easier reference.
*   `free <name>`: Deallocates a named memory block.
*   `status`: Displays the current status of the allocator, including the free list and a list of all named allocations.
*   `help`: Shows a list of available commands.
*   `exit`: Exits the interactive shell.

## Demo

Here is an example of how to use the interactive shell to demonstrate the "Best Fit" allocation strategy:

```
$ ./build/freelist_demo
Choose allocation strategy (first, best): best
Allocator created with 1048576 bytes using Best-Fit strategy.
--- Custom Allocator Shell ---
Commands:
  alloc <size> [name] - Allocates memory. 'name' is optional.
  free <name>         - Deallocates a named memory block.
  status              - Displays the current memory layout.
  exit                - Exits the shell.
  help                - Shows this help message.
-----------------------------
> alloc 100 block1
Allocated 100 bytes at 0x...
  -> Stored as 'block1'
> alloc 200 block2
Allocated 200 bytes at 0x...
  -> Stored as 'block2'
> alloc 50 block3
Allocated 50 bytes at 0x...
  -> Stored as 'block3'
> status
--- Allocator Status ---
Free List:
  - Block at 0x... | Size: 1048194 bytes
------------------------
Allocated Blocks:
  - Name: 'block1' at 0x... | Total Block Size: 116 bytes
  - Name: 'block2' at 0x... | Total Block Size: 216 bytes
  - Name: 'block3' at 0x... | Total Block Size: 66 bytes
------------------------
> free block2
Freed 'block2' (memory at 0x...)
> status
--- Allocator Status ---
Free List:
  - Block at 0x... | Size: 216 bytes
  - Block at 0x... | Size: 1048194 bytes
------------------------
Allocated Blocks:
  - Name: 'block1' at 0x... | Total Block Size: 116 bytes
  - Name: 'block3' at 0x... | Total Block Size: 66 bytes
------------------------
> alloc 150 block4
Allocated 150 bytes at 0x...
  -> Stored as 'block4'
> status
--- Allocator Status ---
Free List:
  - Block at 0x... | Size: 50 bytes
  - Block at 0x... | Size: 1048194 bytes
------------------------
Allocated Blocks:
  - Name: 'block1' at 0x... | Total Block Size: 116 bytes
  - Name: 'block3' at 0x... | Total Block Size: 66 bytes
  - Name: 'block4' at 0x... | Total Block Size: 166 bytes
------------------------
> exit
Exiting.
```

In this demo:
1. We start the allocator with the "Best Fit" strategy.
2. We allocate three blocks of different sizes.
3. We free the middle block (`block2`), which creates a 216-byte hole in the memory.
4. We then allocate a new 150-byte block (`block4`). The "Best Fit" strategy chooses the 216-byte free block because it's the smallest block that can fit the allocation, leaving a 50-byte block free.
