cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

file(GLOB inline_mode_hpp_files "./inline_mode/*.hpp")
file(GLOB inline_mode_cpp_files "./inline_mode/*.cpp")
add_executable (examples_inline_mode ${inline_mode_cpp_files} ${inline_mode_hpp_files})
target_link_libraries(examples_inline_mode 
    anyxx::anyxx_warnings
    anyxx::anyxx_options
    anyxx
    Catch2::Catch2WithMain
)
set_target_properties(examples_inline_mode PROPERTIES FOLDER "_examples")

catch_discover_tests(examples_inline_mode)
