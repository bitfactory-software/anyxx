if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

message( "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})

file(GLOB_RECURSE virtual_void_headers "../include/virtual_void/*.hpp")

add_library(virtual_void_lib ${virtual_void_headers})
target_sources(virtual_void_lib
  PUBLIC
    FILE_SET CXX_MODULES FILES
    "virtual_void.cppm"  
)
target_compile_definitions(virtual_void_lib PUBLIC VV_STATIC)

add_library(virtual_void_shared SHARED ${virtual_void_headers})
target_sources(virtual_void_shared
  PUBLIC
    FILE_SET CXX_MODULES FILES
    "virtual_void.cppm"  
)
target_compile_definitions(virtual_void_shared PUBLIC VV_SHARED)

