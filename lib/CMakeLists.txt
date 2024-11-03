if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

add_library(virtual_void)
target_sources(virtual_void
  PUBLIC
    FILE_SET CXX_MODULES FILES
    "virtual_void.cppm"
)
