if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()


set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE test_all_files "*.cpp")

# Add source to this project's executable.
add_executable (test_all ${test_all_files})

target_link_libraries(test_all 
    virtual_void_shared
    Catch2::Catch2WithMain
)

enable_testing()

include(Catch)
catch_discover_tests(test_all)

