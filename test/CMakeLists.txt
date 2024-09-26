# CMakeList.txt : CMake project for perfect_hash, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (typeid_hash_test "include/catch.cpp" "typeid_hash_test.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET typeid_hash_test PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
