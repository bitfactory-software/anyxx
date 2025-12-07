cmake_minimum_required(VERSION 3.15...3.23)

file(GLOB hpp_files "./*.hpp")
file(GLOB cpp_files "./*.cpp")
add_library(component_base SHARED ${hpp_files} ${cpp_files})
set_target_properties(component_base PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(component_base 
    anyxx::anyxx_warnings
    anyxx::anyxx_options
    anyxx)

target_compile_definitions(component_base PUBLIC ANY_DLL_MODE) # ANY_DLL_MODE works also for  static builds!
if (MSVC)
    target_compile_definitions(component_base PRIVATE COMPONENT_BASE_SHARED)
endif()
