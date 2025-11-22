include(GenerateExportHeader)

add_library(anyxx INTERFACE anyxx.hpp)
add_library(anyxx::anyxx ALIAS anyxx)

if (MSVC)
    if( NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message("add_compile_options(/Zc:preprocessor)")
        target_compile_options(anyxx INTERFACE /Zc:preprocessor)
    endif()
endif()

# target_link_libraries(anyxx 
#    INTERFACE anyxx_options anyxx_warnings)
target_include_directories(anyxx ${WARNING_GUARD} 
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
              $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>)
target_compile_features(anyxx INTERFACE cxx_std_23)
set_target_properties(
  anyxx
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

# generate_export_header(anyxx EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/anyxx/anyxx_export.hpp)

