if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

message( "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})

file(GLOB_RECURSE virtual_void_headers "../include/virtual_void/*.hpp")

add_library(virtual_void ${virtual_void_headers})
target_sources(virtual_void
  PUBLIC
    FILE_SET CXX_MODULES FILES
    "virtual_void.cppm"  
)
if (MSVC)
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(virtual_void PUBLIC VV_SHARED)
    else()
        target_compile_definitions(virtual_void PUBLIC VV_STATIC)
    endif()
endif()

