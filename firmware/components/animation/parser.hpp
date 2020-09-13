#pragma once
#include "animation.hpp"
#include "animation/parser.hpp"

namespace firmware {
	/** Animation parser class
	 *
	 * This implementation directly writes to global animations arrays.
	 *
	 * \note This class should be a local variable and must not be long-lived
	 *       because it acquire firmware::animation_lock. This also mean that you
	 *       may not instanciate more than 1 instance.
	 *       In short, it's only meant to be used in HTTP callback and any way to
	 *       upload a new animation.
	 */
	class parser: public animation::parser {
		/** Lock-guard on the global firmware::animation_lock
		 *
		 * This lock ensure that nobody else is writing to the animation including
		 * the animation engine itself.
		 */
		std::lock_guard<std::mutex> lock_guard;
		
		/** Index of next state to erase
		 */
		unsigned int next_led_state_index = 0;
		/** Index of next stop to erase
		 * \todo TODO Replace by a uint16_t
		 */
		unsigned int next_stop_index = 0;
		
		int emit_init_state(animation::led_state& state) override;
		int emit_stop(animation::stop<animation::basic_rgbw<double>> &stop) override;
		public:
			parser(void) : lock_guard(firmware::animation_lock) {};
			yajl_status complete_parse(void);
	};
}
