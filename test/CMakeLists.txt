if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

include_directories([SYSTEM] "D:/BitFactory/Blog/yomm2/include")
include_directories([SYSTEM] "D:/BitFactory/third_party_2024/vcpkg/installed/x64-windows/include")

find_package(Catch2 3 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_executable (test 
    "_class_hierarchy_test.cpp"
    "_erased_interface_test.cpp"
    "_typeid_cast_test.cpp"
    "_interface_const_correct.cpp"
    "_interface_cast.cpp"
    "_open_method_typeid_test.cpp"
    "_typeid_hash_test.cpp"
    "01_Callback_Inheritance.cpp"
    "02_Callback_TypeErased.cpp"
    "03_Sink_Inheritance.cpp"
    "03_Sink_Inheritance_modern.cpp"
    "04_Sink_TypeErased.cpp"
    "05_Sink_TypeErased_w_any_dispatch.cpp"
    "07_Sink_TypeErased_w_lifetime.cpp"
    "20_Tree_OO.cpp"
    "20_Tree_YOMM2.cpp"
    "21_Tree_TE_dispatch_via_any.cpp"
    "21_Tree_TE_dispatch_via_any_and_type_index.cpp"
    "21_Tree_TE_dispatch_via_m_table.cpp"
    "21_Tree_TE_dispatch_via_type_info.cpp"
    "21_Tree_TE_erased_interface.cpp"
    "_lifetime_erased.cpp" "_erased_std_emulated.cpp"  "_open_method_m_table_test.cpp" "_typeid_factory_test.cpp" "_lifetime_m_table.cpp" "_proxy_prototype.cpp" "_erased_interface_lifetime_convert.cpp" "_open_type_test.cpp" "05_Sink_TypeErased_w_any_cast.cpp" "05_Sink_TypeErased_w_any_dispach_simple.cpp" "_erased_lifetime_data_test.cpp" "_lifetime_typeid_.cpp" "_erased_interface_template_test.cpp" "_call_operator.cpp" "_subscript_operator.cpp")

target_link_libraries(test 
    virtual_void
    Catch2::Catch2WithMain
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

set(UNIT_TEST test)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${UNIT_TEST}
)
