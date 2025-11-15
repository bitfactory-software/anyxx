include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(anyxx_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT anyxx_is_top)
    message("anyxx -> no dependecies required")
    return()
  endif()

  message("cpm add packages...") 
  set(CMAKE_FOLDER __3rdParty)

#  if(NOT TARGET fmtlib::fmtlib)
#    cpmaddpackage("gh:fmtlib/fmt#11.1.4")
#  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.8.1")
  endif()

  if(NOT TARGET tools::tools)
    cpmaddpackage("gh:lefticus/tools#update_build_system")
  endif()

  unset(CMAKE_FOLDER)

endfunction()
