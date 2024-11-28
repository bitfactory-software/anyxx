if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

# Add source to this project's executable.
add_executable (tutorial___1 
    "tutorial___1.cpp"
    "tutorial___1.md"
)
add_executable (tutorial___2
    "tutorial___2.cpp"
    "tutorial___2.md"
)
add_executable (tutorial__30
    "tutorial__30.cpp"
    "tutorial__30.md"
)
add_executable (tutorial__31
    "tutorial__31.cpp"
    "tutorial__31.md"
)
add_executable (tutorial__32
    "tutorial__32.cpp"
    "tutorial__32.md"
)
add_executable (tutorial__33
    "tutorial__33.cpp"
    "tutorial__33.md"
)

target_link_libraries(tutorial___1 virtual_void Catch2::Catch2WithMain)
target_link_libraries(tutorial___2 virtual_void Catch2::Catch2WithMain)
target_link_libraries(tutorial__30 virtual_void Catch2::Catch2WithMain)
target_link_libraries(tutorial__31 virtual_void Catch2::Catch2WithMain)
target_link_libraries(tutorial__32 virtual_void Catch2::Catch2WithMain)
target_link_libraries(tutorial__33 virtual_void Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(tutorial___1)
catch_discover_tests(tutorial___2)
catch_discover_tests(tutorial__30)
catch_discover_tests(tutorial__31)
catch_discover_tests(tutorial__32)
catch_discover_tests(tutorial__33)
