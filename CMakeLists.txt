cmake_minimum_required(VERSION 3.31)

option(anyxx_INSTALL_ONLY					"Install only" OFF)
option(anyxx_BUILD_DOC                      "Build documentation" ON)

# Set the project name and language
project(
  anyxx
  VERSION 0.4.1
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/bitfactory-software/anyxx"
  LANGUAGES CXX C)


if (anyxx_INSTALL_ONLY)  
  add_subdirectory(bit_factory)
  install(FILES bit_factory/anyxx.hpp DESTINATION include/bit_factory)
  install(TARGETS anyxx EXPORT anyxx-config)
  install(EXPORT anyxx-config NAMESPACE bit_factory:: DESTINATION share/anyxx)  
  return()  
endif()

# This template attempts to be "fetch_content"-able
# so that it works well with tools like CPM or other
# manual dependency management

# Only set the cxx_standard if it is not set by someone else
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 23)
endif()

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)


include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)

message("CMAKE_CURRENT_BINARY_DIR:" ${CMAKE_CURRENT_BINARY_DIR})

set(anyxx_is_top)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(anyxx_is_top ON)
endif()
message("anyxx_is_top: " ${anyxx_is_top})

anyxx_setup_options()

anyxx_global_options()
include(Dependencies.cmake)
anyxx_setup_dependencies(anyxx_is_top)

anyxx_local_options()

# don't know if this should be set globally from here or not...
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(GIT_SHA
    "Unknown"
    CACHE STRING "SHA this build was generated from")
string(
  SUBSTRING "${GIT_SHA}"
            0
            8
            GIT_SHORT_SHA)

target_compile_features(anyxx_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(anyxx::anyxx_options ALIAS anyxx_options)
add_library(anyxx::anyxx_warnings ALIAS anyxx_warnings)

#add_library(anyxx::anyxx_options INTERFACE IMPORTED)
#add_library(anyxx::anyxx_warnings INTERFACE IMPORTED)

# configure files based on CMake configuration options
add_subdirectory(configured_files)

# Adding the anyxx:
add_subdirectory(bit_factory)

# Don't even look at tests if we're not top level
if(NOT anyxx_is_top)
  return()
endif()

# Adding the tests:
include(CTest)

if(BUILD_TESTING)
  message("Building Tests. Be sure to check out test/constexpr_tests.cpp for constexpr testing")
  add_subdirectory(test)
  add_subdirectory(examples)
endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()


if(CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

include(cmake/PackageProject.cmake)

# Add other targets that you want installed here, by default we just package the one executable
# we know we want to ship
anyxx_package_project(
  TARGETS
  anyxx
  anyxx_options
  anyxx_warnings
  # FIXME: this does not work! CK
  # PRIVATE_DEPENDENCIES_CONFIGURED project_options project_warnings
)

# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you
# track a build to a specific GIT SHA
set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)


find_package( Doxygen )

if ( DOXYGEN_FOUND )

set( DOXYGEN_FILE_PATTERNS *.hpp *.md )
set( DOXYGEN_OUTPUT_DIRECTORY doxygen )

set( DOXYGEN_EXTRACT_ALL NO )
set( DOXYGEN_HIDE_UNDOC_RELATIONS YES )
set( DOXYGEN_HIDE_UNDOC_CLASSES YES )

set( DOXYGEN_UML_LOOK YES )
set( DOXYGEN_UML_LIMIT_NUM_FIELDS 50 )
set( DOXYGEN_DOT_GRAPH_MAX_NODES 100 )
set( DOXYGEN_DOT_TRANSPARENT YES )
set( DOXYGEN_GENERATE_HTML YES )
set( DOXYGEN_WARN_IF_UNDOCUMENTED NO )

set( DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/README.md" )

doxygen_add_docs( anyxx_doc 
    "${PROJECT_SOURCE_DIR}/bit_factory/" 
    "${PROJECT_SOURCE_DIR}/README.md"
    "${PROJECT_SOURCE_DIR}/README.md"
    "${PROJECT_SOURCE_DIR}/docs/"
    ALL )

else()

message( "Doxygen need to be installed to generate the doxygen documentation" )

endif()