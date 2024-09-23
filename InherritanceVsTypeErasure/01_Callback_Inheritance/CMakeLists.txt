
add_executable (01_Callback_Inheritance "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 01_Callback_Inheritance PROPERTY CXX_STANDARD 20)
endif()

