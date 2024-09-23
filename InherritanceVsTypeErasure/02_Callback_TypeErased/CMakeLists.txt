
add_executable (02_Callback_TypeErased "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 02_Callback_TypeErased PROPERTY CXX_STANDARD 20)
endif()

