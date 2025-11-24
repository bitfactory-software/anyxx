include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(anyxx_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(anyxx_setup_options)
  option(anyxx_ENABLE_HARDENING "Enable hardening" ON)
  option(anyxx_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    anyxx_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    anyxx_ENABLE_HARDENING
    OFF)

  anyxx_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR anyxx_PACKAGING_MAINTAINER_MODE)
    option(anyxx_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(anyxx_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(anyxx_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(anyxx_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(anyxx_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(anyxx_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(anyxx_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(anyxx_ENABLE_PCH "Enable precompiled headers" OFF)
    option(anyxx_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(anyxx_ENABLE_IPO "Enable IPO/LTO" ON)
    option(anyxx_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(anyxx_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(anyxx_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(anyxx_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(anyxx_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(anyxx_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(anyxx_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(anyxx_ENABLE_PCH "Enable precompiled headers" OFF)
    option(anyxx_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      anyxx_ENABLE_IPO
      anyxx_WARNINGS_AS_ERRORS
      anyxx_ENABLE_USER_LINKER
      anyxx_ENABLE_SANITIZER_ADDRESS
      anyxx_ENABLE_SANITIZER_LEAK
      anyxx_ENABLE_SANITIZER_UNDEFINED
      anyxx_ENABLE_SANITIZER_THREAD
      anyxx_ENABLE_SANITIZER_MEMORY
      anyxx_ENABLE_UNITY_BUILD
      anyxx_ENABLE_CLANG_TIDY
      anyxx_ENABLE_CPPCHECK
      anyxx_ENABLE_COVERAGE
      anyxx_ENABLE_PCH
      anyxx_ENABLE_CACHE)
  endif()

  anyxx_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (anyxx_ENABLE_SANITIZER_ADDRESS OR anyxx_ENABLE_SANITIZER_THREAD OR anyxx_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(anyxx_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(anyxx_global_options)
  if(anyxx_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    anyxx_enable_ipo()
  endif()

  anyxx_supports_sanitizers()

  if(anyxx_ENABLE_HARDENING AND anyxx_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR anyxx_ENABLE_SANITIZER_UNDEFINED
       OR anyxx_ENABLE_SANITIZER_ADDRESS
       OR anyxx_ENABLE_SANITIZER_THREAD
       OR anyxx_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${anyxx_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${anyxx_ENABLE_SANITIZER_UNDEFINED}")
    anyxx_enable_hardening(anyxx_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(anyxx_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(anyxx_warnings INTERFACE)
  add_library(anyxx_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  anyxx_set_project_warnings(
    anyxx_warnings
    ${anyxx_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(anyxx_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    anyxx_configure_linker(anyxx_options)
  endif()

  include(cmake/Sanitizers.cmake)
  anyxx_enable_sanitizers(
    anyxx_options
    ${anyxx_ENABLE_SANITIZER_ADDRESS}
    ${anyxx_ENABLE_SANITIZER_LEAK}
    ${anyxx_ENABLE_SANITIZER_UNDEFINED}
    ${anyxx_ENABLE_SANITIZER_THREAD}
    ${anyxx_ENABLE_SANITIZER_MEMORY})

  set_target_properties(anyxx_options PROPERTIES UNITY_BUILD ${anyxx_ENABLE_UNITY_BUILD})

  if(anyxx_ENABLE_PCH)
    target_precompile_headers(
      anyxx_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(anyxx_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    anyxx_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(anyxx_ENABLE_CLANG_TIDY)
    anyxx_enable_clang_tidy(anyxx_options ${anyxx_WARNINGS_AS_ERRORS})
  endif()

  if(anyxx_ENABLE_CPPCHECK)
    anyxx_enable_cppcheck(${anyxx_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(anyxx_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    anyxx_enable_coverage(anyxx_options)
  endif()

  if(anyxx_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(anyxx_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(anyxx_ENABLE_HARDENING AND NOT anyxx_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR anyxx_ENABLE_SANITIZER_UNDEFINED
       OR anyxx_ENABLE_SANITIZER_ADDRESS
       OR anyxx_ENABLE_SANITIZER_THREAD
       OR anyxx_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    anyxx_enable_hardening(anyxx_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
