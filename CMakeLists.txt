cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)

message( "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message("VCPKG_ROOT: " $ENV{VCPKG_ROOT}/)
message( "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})

project(virtual_void CXX)

include(GNUInstallDirs)

find_package(Catch2 3 REQUIRED)

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()
set(CMAKE_CXX_STANDARD 23)

# +++ virtual_void library
file(GLOB_RECURSE VIRTUAL_VOID_HEADERS "./virtual_void/*.hpp")
file(GLOB_RECURSE VIRTUAL_VOID_SOURCES "./virtual_void/*.cpp")
add_library(virtual_void ${VIRTUAL_VOID_HEADERS} ${VIRTUAL_VOID_SOURCES})
target_include_directories(virtual_void PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(virtual_void PRIVATE VV_SHARED)
endif()
target_compile_features(virtual_void PUBLIC cxx_std_23)
# --- virtual_void library

enable_testing()

add_subdirectory ("example_whole_picture")
add_subdirectory ("test")

include(CTest)

message("CMAKE_INSTALL_LIBDIR: " "${CMAKE_INSTALL_LIBDIR}")
message("CMAKE_INSTALL_BINDIR: " "${CMAKE_INSTALL_BINDIR}")
message("CMAKE_INSTALL_DATAROOTDIR: " "${CMAKE_INSTALL_DATAROOTDIR}")
message("CMAKE_CURRENT_BINARY_DIR: " "${CMAKE_CURRENT_BINARY_DIR}")
message("PROJECT_SOURCE_DIR: " "${PROJECT_SOURCE_DIR}")

# Install the library and its headers
install(TARGETS virtual_void
        EXPORT virtual_void_targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(DIRECTORY virtual_void/ # tailing slash is important here!
    DESTINATION include/virtual_void
)

# Generate and install *-targets.cmake
install(EXPORT virtual_void_targets
        FILE virtual_void-targets.cmake
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


