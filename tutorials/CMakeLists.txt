if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_executable (tutorial___1 "../test/include/catch.cpp"
    "tutorial___1.cpp"
 "tutorial___2.cpp" "tutorial___3.cpp")

target_link_libraries(tutorial___1 virtual_void)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

set(UNIT_TEST tutorial___1)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "verify tutorial___1"
     POST_BUILD 
     COMMAND ${UNIT_TEST}
)