cmake_minimum_required(VERSION 3.15...3.23)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16.1")

    set(CMAKE_CXX_STANDARD 26)
    add_compile_options(-freflection)

    project(CmakeConfigPackageTests LANGUAGES CXX)
    include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

    file(GLOB_RECURSE hpp_files "*.hpp")
    file(GLOB_RECURSE cpp_files "*.cpp")
    add_executable (examples_anyxx26 ${cpp_files} ${hpp_files})
    target_link_libraries(examples_anyxx26 
        anyxx::anyxx_warnings
        anyxx::anyxx_options
        bit_factory::anyxx
        Catch2::Catch2WithMain
    )
    if(MSVC)
        set_target_properties(examples_anyxx26 PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
    endif()

    catch_discover_tests(examples_anyxx26)

endif()


