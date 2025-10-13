if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE layer_0_architecture_cpp_files "./layer_0_architecture/*.cpp")
file(GLOB_RECURSE layer_0_architecture_hpp_files "./layer_0_architecture/*.hpp")
add_library(layer_0_architecture ${layer_0_architecture_cpp_files} ${layer_0_architecture_hpp_files})
set_target_properties(layer_0_architecture PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(layer_0_architecture 
    anypp
)
target_compile_definitions(layer_0_architecture PRIVATE ANY_DLL_MODE) # ANY_DLL_MODE works also for  static builds!
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(layer_0_architecture PRIVATE ARCHITECTURE_SHARED)
endif()
set_target_properties(layer_0_architecture PROPERTIES FOLDER "_example_whole_picture")
target_include_directories(layer_0_architecture PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

file(GLOB_RECURSE layer_1_core_cpp_files "./layer_1_core/*.cpp")
file(GLOB_RECURSE layer_1_core_hpp_files "./layer_1_core/*.hpp")
add_library(layer_1_core ${layer_1_core_cpp_files} ${layer_1_core_hpp_files})
set_target_properties(layer_1_core PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(layer_1_core 
    anypp
    layer_0_architecture
)
target_compile_definitions(layer_1_core PRIVATE ANY_DLL_MODE) # ANY_DLL_MODE works also for  static builds!
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(layer_1_core PRIVATE CORE_SHARED)
endif()
set_target_properties(layer_1_core PROPERTIES FOLDER "_example_whole_picture")
target_include_directories(layer_1_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

add_executable (example_1_core example_1_core.cpp)
target_link_libraries(example_1_core 
    anypp
    layer_0_architecture
    layer_1_core
    Catch2::Catch2WithMain
)
target_compile_definitions(example_1_core PRIVATE ANY_DLL_MODE) # ANY_DLL_MODE works also for  static builds!
set_target_properties(example_1_core PROPERTIES FOLDER "_example_whole_picture")

include(Catch)
catch_discover_tests(example_1_core)

