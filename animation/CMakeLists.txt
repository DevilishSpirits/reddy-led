PROJECT(led-animation)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(animation_lib STATIC parser.cpp yajl/src/yajl yajl/src/yajl.c yajl/src/yajl_alloc.c yajl/src/yajl_buf.c yajl/src/yajl_encode.c yajl/src/yajl_gen.c yajl/src/yajl_lex.c yajl/src/yajl_parser.c yajl/src/yajl_tree.c yajl/src/yajl_version.c)
target_include_directories(animation_lib PUBLIC . include include/yajl include2)
