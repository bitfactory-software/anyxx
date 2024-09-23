
add_executable (21_Tree_TE "Tree_TE.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 21_Tree_TE PROPERTY CXX_STANDARD 20)
endif()

