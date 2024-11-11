if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_executable (tutorial___1 
    "../test/include/catch.cpp"
    "tutorial___1.cpp"
)
add_executable (tutorial___2
    "../test/include/catch.cpp"
    "tutorial___2.cpp"
)
add_executable (tutorial___3
    "../test/include/catch.cpp"
    "tutorial___3.cpp"
)

target_link_libraries(tutorial___1 virtual_void)
target_link_libraries(tutorial___2 virtual_void)
target_link_libraries(tutorial___3 virtual_void)

set_property(TARGET test PROPERTY CXX_STANDARD 20)

set(UNIT_TEST tutorial___1)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "verify tutorial___1"
     POST_BUILD 
     COMMAND ${UNIT_TEST}
)

set(UNIT_TEST tutorial___2)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "verify tutorial___2"
     POST_BUILD 
     COMMAND ${UNIT_TEST}
)

set(UNIT_TEST tutorial___3)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "verify tutorial___3"
     POST_BUILD 
     COMMAND ${UNIT_TEST}
)
