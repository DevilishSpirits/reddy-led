#include "animation.hpp"

static_assert(CONFIG_LSTRIP_LED_COUNT + CONFIG_USTRIP_LED_COUNT < CONFIG_STOP_COUNT,"STOP_COUNT must be at least the number of LEDs");
/** The global animation stops holder.
 *
 * All animations are stored in this big array.
 */
animation::stop_array<firmware::color_t,CONFIG_STOP_COUNT> firmware::global_animation;
/** How much frame ticks passed since the last update
 *
 * This global variable is used to compute by how much frames are skipped
 */
unsigned int firmware::current_frame_ticks_forward;

std::array<animation::led_state,CONFIG_LSTRIP_LED_COUNT> firmware::lled_states;
std::array<animation::led_state,CONFIG_USTRIP_LED_COUNT> firmware::uled_states;
