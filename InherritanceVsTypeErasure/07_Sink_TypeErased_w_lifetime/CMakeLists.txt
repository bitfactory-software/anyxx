
add_executable (07_Sink_TypeErased_w_lifetime "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 07_Sink_TypeErased_w_lifetime PROPERTY CXX_STANDARD 20)
endif()

