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
add_executable (tutorial___3
    "tutorial___3.cpp"
    "tutorial___3.md"
)

target_link_libraries(tutorial___1 virtual_void Catch2::Catch2WithMain)
target_link_libraries(tutorial___2 virtual_void Catch2::Catch2WithMain)
target_link_libraries(tutorial___3 virtual_void Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(tutorial___1)
catch_discover_tests(tutorial___2)
catch_discover_tests(tutorial___3)
