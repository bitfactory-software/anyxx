cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)

message("CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("VCPKG_ROOT: " $ENV{VCPKG_ROOT}/)

project(virtual_void CXX)

include(GNUInstallDirs)

include_directories( "./include" )
include_directories( "./src" )

find_package(Catch2 3 REQUIRED)
enable_testing()

add_subdirectory ("src")
add_subdirectory ("test")
add_subdirectory ("tutorials")

include(CTest)

message("CMAKE_INSTALL_LIBDIR: " "${CMAKE_INSTALL_LIBDIR}")
message("CMAKE_INSTALL_BINDIR: " "${CMAKE_INSTALL_BINDIR}")
message("CMAKE_INSTALL_DATAROOTDIR: " "${CMAKE_INSTALL_DATAROOTDIR}")
message("CMAKE_CURRENT_BINARY_DIR: " "${CMAKE_CURRENT_BINARY_DIR}")

# Install the library and its headers
install(TARGETS virtual_void
        EXPORT virtual_void_targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/virtual_void/miu
        FILE_SET HEADERS
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}   # Same as default, could be omitted
        INCLUDES
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#install(FILES ${virtual_void_headers} DESTINATION include)

# Generate and install *-targets.cmake
install(EXPORT virtual_void_targets
        FILE virtual_void-targets.cmake
        CXX_MODULES_DIRECTORY .
        NAMESPACE virtual_void::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/virtual_void)


# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/virtual_void-config.cmake"
"message(\"virtual_void-config.cmake\")\n"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(Catch2 CONFIG REQUIRED)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/virtual_void-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/virtual_void-config.cmake"
        DESTINATION share/virtual_void)


