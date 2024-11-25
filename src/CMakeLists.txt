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

include(GenerateExportHeader)
generate_export_header(virtual_void)
target_sources(virtual_void
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/virtual_void_export.h
)

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
