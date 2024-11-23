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

message("CMAKE_INSTALL_LIBDIR: " ${CMAKE_INSTALL_LIBDIR})
message("CMAKE_INSTALL_BINDIR: " ${CMAKE_INSTALL_BINDIR})

# Install the library and its headers
install(TARGETS virtual_void_lib
        EXPORT virtual_void_targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

#install(FILES ${virtual_void_headers} DESTINATION include)

# Generate and install *-targets.cmake
install(EXPORT virtual_void_lib_targets
        FILE virtual_void_lib-targets.cmake
        NAMESPACE virtual_void::
        DESTINATION share/virtual_void)

#[[
# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/virtual_void-config.cmake"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(Catch2 CONFIG REQUIRED)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/virtual_void-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/my_sample_lib-config.cmake"
        DESTINATION share/virtual_void)
]]
