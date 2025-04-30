cmake_minimum_required(VERSION 3.10)
project(myalloc_consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and link the myalloc package
find_package(myalloc REQUIRED)

# Include directories
include_directories(include)

# List of all solution files
set(SOLUTION_FILES
    src/vector.cpp
    src/hash.cpp
    src/stack.cpp
    src/queue.cpp
    src/dynamic_allocation.cpp
)

# Create an executable for each solution file
foreach(solution_file ${SOLUTION_FILES})
    # Extract the base name (e.g., "vector" from "vector_solution.cpp")
    get_filename_component(base_name ${solution_file} NAME_WE)
    string(REPLACE "_solution" "" base_name ${base_name})

    # Create target name (e.g., "vector_bin")
    set(target_name "${base_name}_bin")

    # Add executable
    add_executable(${target_name} ${solution_file})
    target_link_libraries(${target_name} PRIVATE myalloc)

    # Set output directory if desired
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# Optional: Add an 'all' target that builds everything
add_custom_target(all_solutions DEPENDS
    vector_bin
    hash_bin
    stack_bin
    queue_bin
)