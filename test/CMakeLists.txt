if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

target_include_directories(virtual_void PRIVATE "./include")

file(GLOB_RECURSE test_all_hpp_files "*.hpp")
file(GLOB_RECURSE test_all_cpp_files "*.cpp")

# Add source to this project's executable.
add_executable (test_all ${test_all_cpp_files} ${test_all_hpp_files})

target_link_libraries(test_all 
    virtual_void
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(test_all)
