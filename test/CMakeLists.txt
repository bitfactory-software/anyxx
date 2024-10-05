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
    "_class_hierarchy_test.cpp"
    "_erased_interface_test.cpp"
    "_typeid_cast_test.cpp"
    "_interface_const_correct.cpp"
    "_prototype.cpp"
    "_simple_open_method_test.cpp"
    "_typeid_hash_test.cpp"
    "01_Callback_Inheritance.cpp"
    "02_Callback_TypeErased.cpp"
    "03_Sink_Inheritance.cpp"
    "03_Sink_Inheritance_modern.cpp"
    "04_Sink_TypeErased.cpp"
    "05_Sink_TypeErased_w_any_dispatch.cpp"
    "06_Sink_TypeErased_w_proxy.cpp"
    "07_Sink_TypeErased_w_lifetime.cpp"
    "20_Tree_OO.cpp"
    "20_Tree_YOMM2.cpp"
    "21_Tree_TE_dispatch_via_any.cpp"
    "21_Tree_TE_dispatch_via_any_and_type_index.cpp"
    "21_Tree_TE_dispatch_via_m_table.cpp"
    "21_Tree_TE_dispatch_via_type_info.cpp"
    "21_Tree_TE_erased_interface.cpp"
     "_erased_lifetime.cpp" "_erased_std_emulated.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
