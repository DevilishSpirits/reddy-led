PROJECT(led-animation)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_subdirectory(yajl)
add_library(animation_lib STATIC parser.cpp)
target_include_directories(animation_lib PUBLIC . yajl/src/api)
target_link_libraries(animation_lib PUBLIC yajl_s)
