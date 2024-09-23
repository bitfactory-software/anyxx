
add_executable (06_Sink_TypeErased_w_proxy "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 06_Sink_TypeErased_w_proxy PROPERTY CXX_STANDARD 20)
endif()

