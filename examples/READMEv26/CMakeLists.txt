cmake_minimum_required(VERSION 3.15...3.23)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16.1")

    set(CMAKE_CXX_STANDARD 26)
    add_compile_options(-freflection)

    project(CmakeConfigPackageTests LANGUAGES CXX)
    include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

    file(GLOB hpp_files "*.hpp")
    file(GLOB cpp_files "*.cpp")
    add_executable (READMEv26 ${cpp_files} ${hpp_files})
    target_link_libraries(READMEv26 
        anyxx::anyxx_warnings
        anyxx::anyxx_options
        bit_factory::anyxx
        Catch2::Catch2WithMain
    )
    if(MSVC)
      set_target_properties(READMEv26 PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
    endif()

    # use /Ob2 for benchmarks with Clang-Cl & Cl)
    if(MSVC AND NOT( CMAKE_BUILD_TYPE MATCHES "[Dd]ebug"))
      target_compile_options(READMEv26 PRIVATE  /O2 /Ob2)
    endif()

    catch_discover_tests(READMEv26)

endif()
