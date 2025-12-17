# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Declare the project name
project(FreeListAllocator)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable target
add_executable(freelist_demo
    main.cpp
    allocator.cpp
)

# Optional: Add an include directory if your header was in a separate 'include' folder
# target_include_directories(freelist_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Print a message when the build is configured
message(STATUS "Project configured. Build with 'cmake --build .' and run with './freelist_demo'")
