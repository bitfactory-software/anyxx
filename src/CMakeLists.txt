if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

message( "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})

file(GLOB_RECURSE virtual_void_headers "./*.hpp")

add_library(virtual_void "virtual_void.cpp")

if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(virtual_void PRIVATE VV_SHARED)
endif()
#[[
if (MSVC)
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(virtual_void PUBLIC VV_SHARED)
    else()
        target_compile_definitions(virtual_void PUBLIC VV_STATIC)
    endif()
endif()
]]

target_compile_features(virtual_void PUBLIC cxx_std_20)
