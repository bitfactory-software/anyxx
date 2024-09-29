
add_executable (05_Sink_TypeErased_w_any_dispatch "05_Sink_TypeErased_w_any_dispatch.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 05_Sink_TypeErased_w_any_dispatch PROPERTY CXX_STANDARD 20)
endif()

