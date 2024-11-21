cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

message("CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

find_package(Catch2 3 REQUIRED)

include_directories( "./include" )

project(virtual_void CXX)
add_subdirectory ("lib")
add_subdirectory ("test")
add_subdirectory ("tutorials")

include(CTest)
include(Catch)

