#pragma once
#include "color.hpp"
#include <vector>
#include <array>
namespace animation {
	/** Animation stop
	 *
	 * This store a kind of keyframe in the animation set.
	 */
	template <typename colorT> struct stop {
		/** The color of this stop
		 *
		 */
		colorT color;
		/** The next color stop after this one
		 */
		uint16_t next_index;
		/** The duration of this stop in 1/10s
		 */
		uint8_t duration;
	};
	/** Animation set (dynamic size)
	 *
	 * This vector hold animations stop.
	 */
	template <typename colorT>
	class stop_vector: public std::vector<animation::stop<colorT>> {
		public:
			typedef colorT color;
			typedef animation::stop<colorT> stop;
	};
	/** Animation set (static size)
	 *
	 * This array hold animations stop.
	 */
	template <typename colorT, std::size_t length>
	class stop_array: public std::array<animation::stop<colorT>,length> {
		public:
			typedef colorT color;
			typedef animation::stop<colorT> stop;
	};
	/** The current state of a LED
	 *
	 * It must be paired with a #stop container.
	 */
	struct led_state {
		/** The remaining duration of this stop in 1/10s
		 *
		 * \see stop::duration
		 */
		uint8_t remaining;
		/** The index of the runnning stop
		 */
		uint16_t current;
		
		template <typename T>
		void update(const T& stops, unsigned int steps) {
			while (remaining < steps) {
				const typename T::stop &current_stop = stops[current];
				steps -= remaining;
				current = current_stop.next_index;
				remaining = stops[current].duration;
			}
			remaining -= steps;
		}
	};
	typedef std::vector<led_state> led_state_vector;
	typedef std::vector<led_state> led_state_array;
}
