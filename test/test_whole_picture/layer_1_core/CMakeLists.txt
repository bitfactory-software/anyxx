if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 23)

file(GLOB hpp_files "*.hpp")
file(GLOB cpp_files "*.cpp")
add_library(example_1_architecture ${hpp_files} ${cpp_files})
set_target_properties(example_1_architecture PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(example_1_architecture 
    anyxx
)
target_include_directories(component_base PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
)
if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(component_base PRIVATE COMPONENT_BASE_SHARED)
endif()
