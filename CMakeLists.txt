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
message( "CMAKE_PREFIX_PATH:" ${CMAKE_PREFIX_PATH})

project(anyxx CXX)

include(GNUInstallDirs)

message("VCPKG_ROOT: " $ENV{VCPKG_ROOT}/)
find_package(Catch2 3 REQUIRED)

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()
set(CMAKE_CXX_STANDARD 23)

# +++ anyxx library
file(GLOB_RECURSE anyxx_HEADERS "./anyxx/*.hpp")
file(GLOB_RECURSE anyxx_SOURCES "./anyxx/*.cpp")
add_library(anyxx INTERFACE ${anyxx_HEADERS} ${anyxx_SOURCES})
target_include_directories(anyxx INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)
set_target_properties(anyxx PROPERTIES FOLDER "anyxx")
# --- anyxx library

# +++ coro_callback library
file(GLOB_RECURSE coro_callback_HEADERS "./coro_callback/*.hpp")
file(GLOB_RECURSE coro_callback_SOURCES "./coro_callback/*.cpp")
add_library(coro_callback INTERFACE ${coro_callback_HEADERS} ${coro_callback_SOURCES})
target_include_directories(coro_callback INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)
set_target_properties(coro_callback PROPERTIES FOLDER "coro_callback")
# --- coro_callback library


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

# Install the anyxx library and its headers
install(TARGETS anyxx
        EXPORT anyxx_targets
)

install(DIRECTORY anyxx/ # tailing slash is important here!
    DESTINATION include/anyxx
)

# Generate and install *-targets.cmake
install(EXPORT anyxx_targets
        FILE anyxx-targets.cmake
        NAMESPACE anyxx::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/anyxx)


# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/anyxx-config.cmake"
"message(\"anyxx-config.cmake\")\n"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(Catch2 CONFIG REQUIRED)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/anyxx-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/anyxx-config.cmake"
        DESTINATION share/anyxx)



