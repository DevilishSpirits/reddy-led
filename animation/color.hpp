#pragma once
#include <limits>
#include <cstdint>
namespace animation {
	/** Common base for color types
	 *
	 * This struct has no field but provide many useful functions.
	 */
	template <typename T> struct basic_color_base {
		/** Map a [0..1] value to the underlaying type
		 */
		static T map(float value) {
			if (std::numeric_limits<T>::is_integer) {
				return value * std::numeric_limits<T>::max();
			} else return value; // It's a float
		}
		/** Map a value of the underlaying type to a [0..1] float
		 */
		static float unmap(T value) {
			if (std::numeric_limits<T>::is_integer) {
				return value / (float)std::numeric_limits<T>::max();
			} else return value; // It's a float
		}
		/** Mixer function
		 *
		 * This function make the color mixing using remaining and duration
		 */
		static T mix(T from, T to, int64_t remaining, int64_t duration) {
			const int64_t elapsed = duration - remaining;
			return (from * remaining + to * elapsed)/duration;
		}
	};
	/** Basic RGB value
	 *
	 * This template store a RGB value using the given type. Constructors take a
	 * floating point value in the [0..1] range that's automatically map()ed.
	 *
	 * The white component is discarded.
	 */
	template <typename T> struct basic_rgb: public basic_color_base<T> {
		T red;
		T green;
		T blue;
		/** Mixer function
		 *
		 * This function make the color mixing using remaining and duration
		 */
		static basic_rgb mix(const basic_rgb& from, const basic_rgb& to, int64_t remaining, int64_t duration) {
			basic_rgb result;
			const int64_t elapsed = duration - remaining;
			result.red = (from.red * remaining + to.red * elapsed)/duration;
			result.green = (from.green * remaining + to.green * elapsed)/duration;
			result.blue = (from.blue * remaining + to.blue * elapsed)/duration;
			return result;
		}
		
		basic_rgb(float red, float green, float blue, float white) : red(this->map(red)), green(this->map(green)), blue(this->map(blue)) {};
		basic_rgb(float red, float green, float blue) : red(this->map(red)), green(this->map(green)), blue(this->map(blue)) {};
		basic_rgb(void) = default;
	};
	/** Basic RGBW value
	 *
	 * This template store a RGB value using the given type. Constructors take a
	 * floating point value in the [0..1] range that's automatically map()ed.
	 */
	template <typename T> struct basic_rgbw: public basic_color_base<T> {
		T red;
		T green;
		T blue;
		T white;
		/** Mixer function
		 *
		 * This function make the color mixing using remaining and duration
		 */
		static basic_rgbw mix(const basic_rgbw &from, const basic_rgbw & to, int64_t remaining, int64_t duration) {
			basic_rgbw result;
			const int64_t elapsed = duration - remaining;
			result.red = (from.red * remaining + to.red * elapsed)/duration;
			result.green = (from.green * remaining + to.green * elapsed)/duration;
			result.blue = (from.blue * remaining + to.blue * elapsed)/duration;
			result.white = (from.white * remaining + to.white * elapsed)/duration;
			return result;
		}
		basic_rgbw(float red, float green, float blue, float white) : red(this->map(red)), green(this->map(green)), blue(this->map(blue)), white(this->map(white)) {};
		basic_rgbw(float red, float green, float blue) : red(this->map(red)), green(this->map(green)), blue(this->map(blue)), white(0) {};
		basic_rgbw(void) = default;
	};
}
