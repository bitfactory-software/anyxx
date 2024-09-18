
add_executable (5_Sink_TypeErased_w_Type_info_dispatch "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 5_Sink_TypeErased_w_Type_info_dispatch PROPERTY CXX_STANDARD 20)
endif()

