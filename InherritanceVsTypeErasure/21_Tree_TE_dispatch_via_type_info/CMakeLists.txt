
add_executable (21_Tree_TE_dispatch_via_type_info "21_Tree_TE_dispatch_via_type_info.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 21_Tree_TE_dispatch_via_type_info PROPERTY CXX_STANDARD 20)
endif()

