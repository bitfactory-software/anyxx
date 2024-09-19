
# Add source to this project's executable.
add_executable (example "main.cpp" "simple_open_method/phash/phash_table")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET example PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
