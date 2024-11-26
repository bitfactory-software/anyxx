if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

message( "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})

file(GLOB_RECURSE VIRTUAL_VOID_HEADERS "./*.hpp")

add_library(virtual_void ${VIRTUAL_VOID_HEADERS} "virtual_void.cpp")

target_include_directories(virtual_void PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(virtual_void PRIVATE VV_SHARED)
endif()

target_compile_features(virtual_void PUBLIC cxx_std_20)
