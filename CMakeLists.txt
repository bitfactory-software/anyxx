cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()


include(../third_party_2025/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(Catch2 3 REQUIRED)

include_directories( "./include" )

project(virtual_void CXX)
add_subdirectory ("lib")
add_subdirectory ("test")
add_subdirectory ("tutorials")

include(CTest)
include(Catch)
