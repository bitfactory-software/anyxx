
add_executable (1_Callback_Inheritance "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 1_Callback_Inheritance PROPERTY CXX_STANDARD 20)
endif()

