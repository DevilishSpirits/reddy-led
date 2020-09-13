idf_component_register(
	SRCS "animation.cpp" "parser.cpp"
	INCLUDE_DIRS "."
)
add_subdirectory(animation)
target_link_libraries(${COMPONENT_LIB} PUBLIC animation_lib)
