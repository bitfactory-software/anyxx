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

project(anypp CXX)

include(GNUInstallDirs)

find_package(Catch2 3 REQUIRED)

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()
set(CMAKE_CXX_STANDARD 23)

# +++ anypp library
file(GLOB_RECURSE anypp_HEADERS "./anypp/*.hpp")
file(GLOB_RECURSE anypp_SOURCES "./anypp/*.cpp")
add_library(anypp INTERFACE ${anypp_HEADERS} ${anypp_SOURCES})
target_include_directories(anypp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)
set_target_properties(anypp PROPERTIES FOLDER "anypp")
# --- anypp library

enable_testing()

add_subdirectory ("examples")
add_subdirectory ("example_whole_picture")
add_subdirectory ("test")

include(CTest)

message("CMAKE_INSTALL_LIBDIR: " "${CMAKE_INSTALL_LIBDIR}")
message("CMAKE_INSTALL_BINDIR: " "${CMAKE_INSTALL_BINDIR}")
message("CMAKE_INSTALL_DATAROOTDIR: " "${CMAKE_INSTALL_DATAROOTDIR}")
message("CMAKE_CURRENT_BINARY_DIR: " "${CMAKE_CURRENT_BINARY_DIR}")
message("PROJECT_SOURCE_DIR: " "${PROJECT_SOURCE_DIR}")

# Install the library and its headers
install(TARGETS anypp
        EXPORT anypp_targets
##        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
##        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
##        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(DIRECTORY anypp/ # tailing slash is important here!
    DESTINATION include/anypp
)

# Generate and install *-targets.cmake
install(EXPORT anypp_targets
        FILE anypp-targets.cmake
        NAMESPACE anypp::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/anypp)


# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/anypp-config.cmake"
"message(\"anypp-config.cmake\")\n"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(Catch2 CONFIG REQUIRED)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/anypp-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/anypp-config.cmake"
        DESTINATION share/anypp)


