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
target_compile_definitions(component_base PRIVATE VV_DLL_MODE) # VV_DLL_MODE works also for  static builds!
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(component_base PRIVATE COMPONENT_BASE_SHARED)
endif()
target_compile_features(component_base PUBLIC cxx_std_23)
set_target_properties(component_base PROPERTIES FOLDER "_tests/dll_mode")

file(GLOB tests_dll_mode_hpp_files "./tests_dll_mode/*.hpp")
file(GLOB tests_dll_mode_cpp_files "./tests_dll_mode/*.cpp")
add_executable (tests_dll_mode ${tests_dll_mode_cpp_files} ${tests_dll_mode_hpp_files})
target_link_libraries(tests_dll_mode 
    virtual_void
    component_base
    Catch2::Catch2WithMain
)
target_compile_definitions(component_base PRIVATE VV_DLL_MODE) # VV_DLL_MODE works also for  static builds!
set_target_properties(tests_dll_mode PROPERTIES FOLDER "_tests/dll_mode")

file(GLOB tests_inline_mode_hpp_files "./tests_inline_mode/*.hpp")
file(GLOB tests_inline_mode_cpp_files "./tests_inline_mode/*.cpp")
add_executable (tests_inline_mode ${tests_inline_mode_cpp_files} ${tests_inline_mode_hpp_files})
target_link_libraries(tests_inline_mode 
    virtual_void
    Catch2::Catch2WithMain
)
set_target_properties(tests_inline_mode PROPERTIES FOLDER "_tests/inline_mode")

include(Catch)
catch_discover_tests(tests_inline_mode)
