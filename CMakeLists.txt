cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

include(../third_party_2025/vcpkg/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /third_party_2025/vcpkg/scripts/)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ../third_party_2025/vcpkg/installed/x64-windows/lib)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ../third_party_2025/vcpkg/installed/x64-windows/lib/manual-link)

include_directories( "./include" )
include_directories( "./test/include" )

project(virtual_void CXX)
add_subdirectory ("test")
add_subdirectory ("lib")
add_subdirectory ("tutorials")
