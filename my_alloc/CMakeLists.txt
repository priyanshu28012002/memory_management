cmake_minimum_required(VERSION 3.10)
project(my_alloc VERSION 1.0 LANGUAGES CXX)

# Set C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing framework
enable_testing()

# Find required packages
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# Include directories
include_directories(include)

# === Build the shared library ===
add_library(myalloc SHARED src/my_alloc.cpp)
target_include_directories(myalloc PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
set_target_properties(myalloc PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "include/my_alloc.hpp"
)

# === Unit tests using Google Test ===
add_executable(unit_test test/unit_test.cpp)
target_link_libraries(unit_test PRIVATE myalloc Threads::Threads GTest::GTest GTest::Main)
add_test(NAME UnitTests COMMAND unit_test)

# === Installation ===
install(TARGETS myalloc
    EXPORT myallocTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
)

install(EXPORT myallocTargets
    FILE myallocTargets.cmake
    NAMESPACE myalloc::
    DESTINATION lib/cmake/myalloc
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/myallocConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/myallocConfig.cmake
    INSTALL_DESTINATION lib/cmake/myalloc
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/myallocConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/myallocConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/myallocConfigVersion.cmake
    DESTINATION lib/cmake/myalloc
)

# === Custom build targets ===
add_custom_target(run_tests
    COMMAND unit_test
    DEPENDS unit_test
    COMMENT "Running unit tests"
)