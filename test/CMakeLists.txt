# CMakeList.txt : CMake project for perfect_hash, include source and define
# project specific logic here.
#

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

include_directories([SYSTEM] "D:/BitFactory/Blog/yomm2/include")
include_directories([SYSTEM] "D:/BitFactory/third_party_2024/vcpkg/installed/x64-windows/include")

# Add source to this project's executable.
add_executable (test "include/catch.cpp"
    "20_Tree_OO.cpp"
    "20_Tree_YOMM2.cpp"
    "dynamic_interface_test.cpp"
    "typeid_hash_test.cpp" 
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
