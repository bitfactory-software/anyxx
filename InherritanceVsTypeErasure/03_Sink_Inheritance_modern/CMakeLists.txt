
add_executable (03_Sink_Inheritance_modern "03_Sink_Inheritance_modern.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 03_Sink_Inheritance_modern PROPERTY CXX_STANDARD 20)
endif()

