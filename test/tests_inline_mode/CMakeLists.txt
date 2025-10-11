if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 23)

file(GLOB component_base_hpp_files "./component_base/*.hpp")
file(GLOB component_base_cpp_files "./component_base/*.cpp")
add_library(component_base ${component_base_hpp_files} ${component_base_cpp_files})
set_target_properties(component_base PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(component_base 
    virtual_void
)
target_include_directories(component_base PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(component_base PRIVATE COMPONENT_BASE_SHARED)
endif()
set_target_properties(component_base PROPERTIES FOLDER "_test_all")



file(GLOB test_all_hpp_files "*.hpp")
file(GLOB test_all_cpp_files "*.cpp")
add_executable (test_all ${test_all_cpp_files} ${test_all_hpp_files})
target_link_libraries(test_all 
    virtual_void
    component_base
    Catch2::Catch2WithMain
)
set_target_properties(test_all PROPERTIES FOLDER "_test_all")

include(Catch)
catch_discover_tests(test_all)
