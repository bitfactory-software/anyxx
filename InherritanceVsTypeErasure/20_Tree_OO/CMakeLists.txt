
add_executable (20_Tree_OO "20_Tree_OO.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 20_Tree_OO PROPERTY CXX_STANDARD 20)
endif()

