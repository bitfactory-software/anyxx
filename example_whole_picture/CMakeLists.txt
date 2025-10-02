if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 23)

file(GLOB layer_0_architecture_hpp_files "./layer_0_architecture/*.hpp")
file(GLOB layer_0_architecture_cpp_files "./layer_0_architecture/*.cpp")
add_library(layer_0_architecture layer_0_architecture/architecture.hpp layer_0_architecture/architecture.cpp )
set_target_properties(layer_0_architecture PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(layer_0_architecture 
    virtual_void
)
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(layer_0_architecture PRIVATE ARCHITECTURE_SHARED)
endif()
##set_target_properties(layer_0_architecture PROPERTIES FOLDER "example_whole_picture")
target_include_directories(layer_0_architecture PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

add_library(layer_1_core layer_1_core/core.hpp layer_1_core/core.cpp )
set_target_properties(layer_1_core PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(layer_1_core 
    virtual_void
    layer_0_architecture
)
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(layer_1_core PRIVATE CORE_SHARED)
endif()
##set_target_properties(layer_1_core PROPERTIES FOLDER "example_whole_picture")
target_include_directories(layer_1_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

add_executable (example_1_core example_1_core.cpp)
target_link_libraries(example_1_core 
    virtual_void
    layer_0_architecture
    layer_1_core
    Catch2::Catch2WithMain
)
##set_target_properties(example_1_core PROPERTIES FOLDER "example_whole_picture")

include(Catch)
catch_discover_tests(example_1_core)

