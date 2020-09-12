PROJECT(led-anim-analyze)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_subdirectory(animation)
add_executable(led-anim-analyze main.cpp)
target_include_directories(led-anim-analyze PUBLIC animation)
target_link_libraries(led-anim-analyze PUBLIC animation_lib)
