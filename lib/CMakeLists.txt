if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE virtual_void_headers "../include/virtual_void/*.h")

add_library(virtual_void ${virtual_void_headers})
target_sources(virtual_void
  PUBLIC
    FILE_SET CXX_MODULES FILES
    "virtual_void.cppm"  
)
target_compile_definitions(virtual_void PUBLIC VV_STATIC)

add_library(virtual_void_shared SHARED ${virtual_void_headers})
target_sources(virtual_void_shared
  PUBLIC
    FILE_SET CXX_MODULES FILES
    "virtual_void.cppm"  
)
target_compile_definitions(virtual_void_shared PUBLIC VV_SHARED)