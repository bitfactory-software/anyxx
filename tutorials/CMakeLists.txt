if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)
find_package(Catch2 3 REQUIRED)

# Add source to this project's executable.
add_executable (tutorial___1 
    "tutorial___1.cpp"
)
add_executable (tutorial___2
    "tutorial___2.cpp"
)
add_executable (tutorial___3
    "tutorial___3.cpp"
)

target_link_libraries(tutorial___1 virtual_void_lib Catch2::Catch2WithMain)
target_link_libraries(tutorial___2 virtual_void_lib Catch2::Catch2WithMain)
target_link_libraries(tutorial___3 virtual_void_lib Catch2::Catch2WithMain)

enable_testing()

include(Catch)
catch_discover_tests(tutorial___1)
catch_discover_tests(tutorial___2)
catch_discover_tests(tutorial___3)
