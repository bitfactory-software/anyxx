
add_executable (04_Sink_TypeErased "04_Sink_TypeErased.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 04_Sink_TypeErased PROPERTY CXX_STANDARD 20)
endif()

