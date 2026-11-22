cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

file(GLOB hpp_files "*.hpp")
file(GLOB cpp_files "*.cpp")
add_executable (READMEv23 ${cpp_files} ${hpp_files})
target_link_libraries(READMEv23 
    anyxx::anyxx_warnings
    anyxx::anyxx_options
    bit_factory::anyxx
    Catch2::Catch2WithMain
)
if(MSVC)
  set_target_properties(READMEv23 PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

# use /Ob2 for benchmarks with Clang-Cl & Cl)
if(MSVC AND NOT( CMAKE_BUILD_TYPE MATCHES "[Dd]ebug"))
  target_compile_options(READMEv23 PRIVATE  /O2 /Ob2)
endif()

catch_discover_tests(READMEv23)
