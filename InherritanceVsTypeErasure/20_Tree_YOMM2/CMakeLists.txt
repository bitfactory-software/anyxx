
include_directories([SYSTEM] "D:/BitFactory/Blog/yomm2/include")
include_directories([SYSTEM] "D:/BitFactory/third_party_2024/vcpkg/installed/x64-windows/include")

add_executable (20_Tree_YOMM2 "20_Tree_YOMM2.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 20_Tree_YOMM2 PROPERTY CXX_STANDARD 20)
endif()

