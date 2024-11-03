cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

include_directories( "./include" )

project(virtual_void CXX)
add_subdirectory ("test")


# Turning off extensions avoids an issue with the clang 16 compiler
# clang 17 and greater can avoid this setting
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the version of C++ for the project
set(CMAKE_CXX_STANDARD 20)
# Create a library
add_library(foo)
# Add the module file to the library
target_sources(foo
  PUBLIC
    FILE_SET CXX_MODULES FILES
      foo.cppm
)
# Create an executable
add_executable(hello main.cxx)
# Link to the library foo
target_link_libraries(hello foo)