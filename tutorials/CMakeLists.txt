if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_executable (tutorial_lifetime___1 "../test/include/catch.cpp"
    "tutorial_lifetime___1.cpp"
)

target_link_libraries(tutorial_lifetime___1 virtual_void)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

set(UNIT_TEST tutorial_lifetime___1)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "verify tutorial_lifetime___1"
     POST_BUILD 
     COMMAND ${UNIT_TEST}
)