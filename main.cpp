#include "allocator.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <limits>

void print_help() {
    std::cout << "--- Custom Allocator Shell ---\n"
              << "Commands:\n"
              << "  alloc <size> [name] - Allocates memory. 'name' is optional.\n"
              << "  free <name>         - Deallocates a named memory block.\n"
              << "  status              - Displays the current memory layout.\n"
              << "  exit                - Exits the shell.\n"
              << "  help                - Shows this help message.\n"
              << "-----------------------------" << std::endl;
}

int main() {
    std::string strategy_input;
    FreeListAllocator::AllocationStrategy strategy;

    while (true) {
        std::cout << "Choose allocation strategy (first, best): ";
        std::cin >> strategy_input;
        if (strategy_input == "first") {
            strategy = FreeListAllocator::AllocationStrategy::FirstFit;
            break;
        } else if (strategy_input == "best") {
            strategy = FreeListAllocator::AllocationStrategy::BestFit;
            break;
        } else {
            std::cout << "Invalid strategy. Please type 'first' or 'best'." << std::endl;
        }
    }

    const std::size_t buffer_size = 1024 * 1024; // 1 MB
    FreeListAllocator allocator(buffer_size, strategy);
    std::cout << "Allocator created with " << buffer_size << " bytes using " 
              << (strategy == FreeListAllocator::AllocationStrategy::FirstFit ? "First-Fit" : "Best-Fit")
              << " strategy." << std::endl;

    print_help();

    std::map<std::string, void*> named_allocations;
    std::string line;
    std::string command;
    
    // Clear the rest of the line after reading strategy
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break; // End of input
        }

        std::stringstream ss(line);
        ss >> command;

        if (command == "exit") {
            break;
        } else if (command == "help") {
            print_help();
        } else if (command == "alloc") {
            std::size_t size = 0;
            std::string name;
            ss >> size;
            ss >> name; // Optional name

            if (size == 0) {
                std::cout << "Usage: alloc <size> [name]" << std::endl;
            } else {
                void* ptr = allocator.allocate(size);
                if (ptr) {
                    std::cout << "Allocated " << size << " bytes at " << ptr << std::endl;
                    if (!name.empty()) {
                        if (named_allocations.count(name)) {
                            std::cout << "Warning: Overwriting named allocation '" << name << "'" << std::endl;
                        }
                        named_allocations[name] = ptr;
                        std::cout << "  -> Stored as '" << name << "'" << std::endl;
                    }
                } else {
                    std::cout << "Allocation failed (out of memory)." << std::endl;
                }
            }
        } else if (command == "free") {
            std::string name;
            ss >> name;
            if (name.empty()) {
                std::cout << "Usage: free <name>" << std::endl;
            } else {
                auto it = named_allocations.find(name);
                if (it != named_allocations.end()) {
                    allocator.deallocate(it->second);
                    std::cout << "Freed '" << name << "' (memory at " << it->second << ")" << std::endl;
                    named_allocations.erase(it);
                } else {
                    std::cout << "Error: No allocation named '" << name << "'" << std::endl;
                }
            }
        } else if (command == "status") {
            allocator.printStatus();
            std::cout << "Allocated Blocks:\n";
            if (named_allocations.empty()) {
                std::cout << "  (none)\n";
            } else {
                for (const auto& pair : named_allocations) {
                    std::size_t size = allocator.getAllocationSize(pair.second);
                    std::cout << "  - Name: '" << pair.first << "' at " << pair.second
                              << " | Total Block Size: " << size << " bytes\n";
                }
            }
            std::cout << "------------------------\n";
        } else if (!command.empty()) {
            std::cout << "Unknown command: '" << command << "'. Type 'help' for a list of commands." << std::endl;
        }
    }

    std::cout << "Exiting." << std::endl;
    return 0;
}