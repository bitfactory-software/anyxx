cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

file(GLOB_RECURSE hpp_files "*.hpp")
file(GLOB_RECURSE cpp_files "*.cpp")
add_executable(tests_inline_mode ${hpp_files} ${cpp_files})
#add_executable(tests_inline_mode _any_template.cpp)
target_link_libraries(
  tests_inline_mode
  PRIVATE anyxx::anyxx_warnings
          anyxx::anyxx_options
          anyxx::anyxx
          Catch2::Catch2WithMain)

# use /Ob2 for benchmarks with Clang-Cl & Cl)
#if (MSVC)
#  target_compile_options(tests_inline_mode PRIVATE /Ob2)
#endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
#catch_discover_tests(
#  tests_inline_mode
#  TEST_PREFIX
#  "tests_inline_mode"
#  REPORTER
#  XML
#  OUTPUT_DIR
#  .
#  OUTPUT_PREFIX
#  "tests_inline_mode"
#  OUTPUT_SUFFIX
#  .xml)

catch_discover_tests(tests_inline_mode)
