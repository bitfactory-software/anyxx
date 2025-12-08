project(CmakeConfigPackageTests LANGUAGES CXX)

file(GLOB_RECURSE layer_0_architecture_cpp_files "./layer_0_architecture/*.cpp")
file(GLOB_RECURSE layer_0_architecture_hpp_files "./layer_0_architecture/*.hpp")
add_library(layer_0_architecture SHARED ${layer_0_architecture_cpp_files} ${layer_0_architecture_hpp_files})
set_target_properties(layer_0_architecture PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(layer_0_architecture PUBLIC ANY_DLL_MODE)
target_compile_definitions(layer_0_architecture PRIVATE ARCHITECTURE_SHARED)
set_target_properties(layer_0_architecture PROPERTIES FOLDER "_example_whole_picture")
target_link_libraries(layer_0_architecture 
  PRIVATE   anyxx::anyxx_warnings
            anyxx::anyxx_options
  PUBLIC    anyxx::anyxx
)
target_include_directories(layer_0_architecture INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

file(GLOB_RECURSE layer_1_core_cpp_files "./layer_1_core/*.cpp")
file(GLOB_RECURSE layer_1_core_hpp_files "./layer_1_core/*.hpp")
add_library(layer_1_core SHARED ${layer_1_core_cpp_files} ${layer_1_core_hpp_files})
set_target_properties(layer_1_core PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(layer_1_core 
  PRIVATE   anyxx::anyxx_warnings
            anyxx::anyxx_options
  PUBLIC    layer_0_architecture
)
target_compile_definitions(layer_1_core PRIVATE CORE_SHARED)
set_target_properties(layer_1_core PROPERTIES FOLDER "_example_whole_picture")
target_include_directories(layer_1_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

file(GLOB_RECURSE layer_2_collision_cpp_files "./layer_2_collision/*.cpp")
file(GLOB_RECURSE layer_2_collision_hpp_files "./layer_2_collision/*.hpp")
add_library(layer_2_collision SHARED ${layer_2_collision_cpp_files} ${layer_2_collision_hpp_files})
set_target_properties(layer_2_collision PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(layer_2_collision PUBLIC ANY_DLL_MODE)
target_link_libraries(layer_2_collision 
  PRIVATE   anyxx::anyxx_warnings
            anyxx::anyxx_options
  PUBLIC    layer_1_core
)
target_compile_definitions(layer_2_collision PRIVATE COLLISION_SHARED)
set_target_properties(layer_2_collision PROPERTIES FOLDER "_example_whole_picture")
target_include_directories(layer_2_collision PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

file(GLOB_RECURSE test_cpp_files "./*test*.cpp")
add_executable (test_whole_picture ${test_cpp_files})
target_link_libraries(test_whole_picture 
  PRIVATE   anyxx::anyxx_warnings
            anyxx::anyxx_options
            layer_0_architecture
            layer_1_core
            layer_2_collision
            Catch2::Catch2WithMain
)
set_target_properties(test_whole_picture PROPERTIES FOLDER "_example_whole_picture")

catch_discover_tests(test_whole_picture)

