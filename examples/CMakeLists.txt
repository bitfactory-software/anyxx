cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

file(GLOB inline_mode_hpp_files "./inline_mode/*.hpp")
file(GLOB inline_mode_cpp_files "./inline_mode/*.cpp")
add_executable (examples_inline_mode ${inline_mode_cpp_files} ${inline_mode_hpp_files} ./inline_mode/README.cpp)
target_link_libraries(examples_inline_mode 
    anyxx::anyxx_warnings
    anyxx::anyxx_options
    bit_factory::anyxx
    Catch2::Catch2WithMain
)
if(MSVC)
  set_target_properties(examples_inline_mode PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

# use /Ob2 for benchmarks with Clang-Cl & Cl)
if(MSVC AND NOT( CMAKE_BUILD_TYPE MATCHES "[Dd]ebug"))
  target_compile_options(examples_inline_mode PRIVATE /Ob2)
endif()

catch_discover_tests(examples_inline_mode)
