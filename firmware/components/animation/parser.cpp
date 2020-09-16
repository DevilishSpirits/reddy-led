#include "parser.hpp"
#include "animation/parser.hpp"
#include <sstream>

static animation::led_state &led_state_from_json_index(unsigned int index)
{
	if (index >= firmware::lled_states.size())
		return firmware::uled_states[index - firmware::lled_states.size()];
	else return firmware::lled_states[index];
}
int firmware::parser::emit_init_state(animation::led_state& state)
{
	const uint16_t index = next_led_state_index++;
	if (index >= lled_states.size() + uled_states.size()) {
		// Ignore out-of-strip indexes 
		return 1;
	}
	led_state_from_json_index(index) = state;
	return 1;
}
int firmware::parser::emit_stop(animation::stop<animation::basic_rgbw<double>> &stop)
{
	const auto index = next_stop_index++;
	if (index >= global_animation.size()) {
		parse_error = "Too much elements in the \"stops\" array. The maximum is " + std::to_string(global_animation.size()) + " elements.";
		return 0;
	}
	global_animation[index].color.red = firmware::color_t::map(stop.color.red);
	global_animation[index].color.green = firmware::color_t::map(stop.color.green);
	global_animation[index].color.blue = firmware::color_t::map(stop.color.blue);
	//global_animation[index].color.white = firmware::color_t::map(stop.color.white);
	global_animation[index].duration = stop.duration;
	global_animation[index].next_index = stop.next_index;
	return 1;
}
yajl_status firmware::parser::complete_parse(void)
{
	// Finish scan
	yajl_status yajl_result = animation::parser::complete_parse();
	if (yajl_result != yajl_status_ok)
		return yajl_result;
	// Sanity check
	bool sane = true;
	std::ostringstream error_string(parse_error);
	for (auto i = 0; i < lled_states.size(); i++)
		if (lled_states[i].current >= global_animation.size()) {
			error_string << "\"stop\" of the " << i << "th \"init\" is greater than the number of \"stops\" (" << lled_states[i].current << " must be less than " << global_animation.size() << ")\n";
			sane = false;
		}
	for (auto i = 0; i < lled_states.size(); i++)
		if (uled_states[i].current >= global_animation.size()) {
			error_string << "\"stop\" of the " << i-lled_states.size() << "th \"init\" is greater than the number of \"stops\" (" << lled_states[i].current << " must be less than " << global_animation.size() << ")\n";
			sane = false;
		}
	for (auto i = 0; i < global_animation.size(); i++)
		if (global_animation[i].next_index >= global_animation.size()) {
			error_string << "\"next_index\" of the " << i << "th \"stops\" is greater than the number of \"stops\" (" << global_animation[i].next_index << " must be less than " << global_animation.size() << ")\n";
			sane = false;
		}
	if (!sane)
		return yajl_status_client_canceled;
	
	/* - Animation is sane - */
	// Repeat the pattern on unset LEDs
	const unsigned int led_count = lled_states.size() + uled_states.size();
	for (auto i = next_led_state_index; i < led_count; i++)
		led_state_from_json_index(i) = led_state_from_json_index(i % next_led_state_index);
	return yajl_status_ok;
}
