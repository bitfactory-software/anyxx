project(CmakeConfigPackageTests LANGUAGES CXX)

file(GLOB_RECURSE layer_0_architecture_cpp_files "./layer_0_architecture/*.cpp")
file(GLOB_RECURSE layer_0_architecture_hpp_files "./layer_0_architecture/*.hpp")
add_library(layer_0_architecture INTERFACE ${layer_0_architecture_cpp_files} ${layer_0_architecture_hpp_files})
set_target_properties(layer_0_architecture PROPERTIES FOLDER "_example_whole_picture")
target_include_directories(layer_0_architecture INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

file(GLOB_RECURSE layer_1_core_cpp_files "./layer_1_core/*.cpp")
file(GLOB_RECURSE layer_1_core_hpp_files "./layer_1_core/*.hpp")
add_library(layer_1_core SHARED ${layer_1_core_cpp_files} ${layer_1_core_hpp_files})
set_target_properties(layer_1_core PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(layer_1_core PUBLIC ANY_DLL_MODE)
target_link_libraries(layer_1_core 
  PRIVATE   anyxx::anyxx_warnings
            anyxx::anyxx_options
            anyxx::anyxx
            layer_0_architecture
)
target_compile_definitions(layer_1_core PRIVATE CORE_SHARED)
set_target_properties(layer_1_core PROPERTIES FOLDER "_example_whole_picture")
target_include_directories(layer_1_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

add_executable (test_whole_picture "test_whole_picture.cpp")
target_link_libraries(test_whole_picture 
  PRIVATE   anyxx::anyxx_warnings
            anyxx::anyxx_options
            anyxx::anyxx
            layer_0_architecture
            layer_1_core
            Catch2::Catch2WithMain
)
set_target_properties(test_whole_picture PROPERTIES FOLDER "_example_whole_picture")

catch_discover_tests(test_whole_picture)

