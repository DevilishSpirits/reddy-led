#pragma once
#include <sdkconfig.h>
#include "../../../animation/animation.hpp"
namespace firmware {
	typedef uint8_t color_val_t;
	typedef animation::basic_rgb<uint8_t> color_t;
	typedef animation::stop<color_t> animation_stop_t;
	
	extern animation::stop_array<firmware::color_t,CONFIG_STOP_COUNT> global_animation;
	extern unsigned int current_frame_ticks_forward;
	
	extern std::array<animation::led_state,CONFIG_LSTRIP_LED_COUNT> lled_states;
	extern std::array<animation::led_state,CONFIG_USTRIP_LED_COUNT> uled_states;
}
