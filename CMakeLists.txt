cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

include_directories( "./include" )

project(virtual_void CXX)
add_subdirectory ("test")
add_subdirectory ("lib")
