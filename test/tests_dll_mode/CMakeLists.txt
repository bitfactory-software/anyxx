cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

file(GLOB_RECURSE hpp_files "*.hpp")
file(GLOB_RECURSE cpp_files "*.cpp")
add_executable(tests_dll_mode ${cpp_files} ${hpp_files})
#add_executable(tests_dll_mode _any_template.cpp)
target_link_libraries(
  tests_dll_mode
  PRIVATE anyxx::anyxx_warnings
          anyxx::anyxx_options
          anyxx::anyxx
          component_base
          Catch2::Catch2WithMain)

add_custom_command(
    TARGET tests_dll_mode
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests_dll_mode> $<TARGET_FILE_DIR:tests_dll_mode>
    COMMAND_EXPAND_LISTS)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
#catch_discover_tests(
#  tests_dll_mode
#  TEST_PREFIX
#  "tests_dll_mode"
#  REPORTER
#  XML
#  OUTPUT_DIR
#  .
#  OUTPUT_PREFIX
#  "tests_dll_mode"
#  OUTPUT_SUFFIX
#  .xml)

catch_discover_tests(tests_dll_mode)
