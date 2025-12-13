cmake_minimum_required(VERSION 3.15...3.23)

file(GLOB hpp_files "./*.hpp")
file(GLOB cpp_files "./*.cpp")

if(MSVC)
  add_library(component_base SHARED ${hpp_files} ${cpp_files})
else()    
  add_library(component_base STATIC ${hpp_files} ${cpp_files})
endif()

target_link_libraries(component_base 
    anyxx::anyxx_warnings
    anyxx::anyxx_options
    anyxx)

target_compile_definitions(component_base PUBLIC ANY_DLL_MODE) # ANY_DLL_MODE works also for  static builds!
target_compile_definitions(component_base PRIVATE COMPONENT_BASE_SHARED)

