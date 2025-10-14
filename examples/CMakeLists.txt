if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 23)

file(GLOB inline_mode_hpp_files "./inline_mode/*.hpp")
file(GLOB inline_mode_cpp_files "./inline_mode/*.cpp")
add_executable (examples_inline_mode ${inline_mode_cpp_files} ${inline_mode_hpp_files})
target_link_libraries(examples_inline_mode 
    anyxx
    Catch2::Catch2WithMain
)
set_target_properties(examples_inline_mode PROPERTIES FOLDER "_examples/inline_mode")

include(Catch)
catch_discover_tests(examples_inline_mode)
