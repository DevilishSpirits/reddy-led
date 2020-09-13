if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
   add_compile_options (-Wno-pointer-arith)
   add_compile_options (-Wno-implicit-fallthrough)
endif ()
idf_component_register(
	SRCS "main.cpp" "web-anim.cpp" # "web.cpp" "web-fill.cpp" "web-udp.cpp"
	INCLUDE_DIRS ""
	REQUIRES ws2812 driver esp_http_server) # esp_netif nvs_flash
