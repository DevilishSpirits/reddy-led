#pragma once
#include <sdkconfig.h>
#include "../../../animation/animation.hpp"
#include <mutex>
namespace firmware {
	typedef uint8_t color_val_t;
	typedef animation::basic_rgb<uint8_t> color_t;
	typedef animation::stop<color_t> animation_stop_t;
	
	extern animation::stop_array<firmware::color_t,CONFIG_STOP_COUNT> global_animation;
	
	extern int64_t current_frame_ticks_forward;
	extern int64_t subframe_difference;
	
	extern std::array<animation::led_state,CONFIG_LSTRIP_LED_COUNT> lled_states;
	extern std::array<animation::led_state,CONFIG_USTRIP_LED_COUNT> uled_states;
	
	const auto frame_step_divider = 100000;
	
	extern std::mutex animation_lock;
}
