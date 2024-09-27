# CMakeList.txt : CMake project for perfect_hash, include source and define
# project specific logic here.
#

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

# Add source to this project's executable.
add_executable (test "include/catch.cpp" "typeid_hash_test.cpp" "dynamic_interface_test.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
