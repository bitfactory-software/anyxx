
add_executable (21_Tree_TE_dispach_via_v_table "21_Tree_TE_dispach_via_v_table.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 21_Tree_TE_dispach_via_v_table PROPERTY CXX_STANDARD 20)
endif()

