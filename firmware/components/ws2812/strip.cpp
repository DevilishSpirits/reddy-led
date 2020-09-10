#include "strip.hpp"
#include <animation.hpp>

// Inspired from https://github.com/espressif/esp-idf/blob/600d542f53ab6540d277fca4716824d168244c8c/examples/peripherals/rmt/led_strip/components/led_strip/src/led_strip_rmt_ws2812.c

/** Strip bit timing informations
 *
 * This array contain rmt_item32_t consts used to produce 0 and 1 bits using the
 * [RMT](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html)
 */
const rmt_item32_t firmware::rmt_bits[2] = {{{15,1,33,0}}/*0*/,/*1*/{{33,1,15,0}}};

/** Send RBG stream
 *
 * This is the translation function used in [rmt_translator_init](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html#_CPPv419rmt_translator_init13rmt_channel_t15sample_to_rmt_t).
 *
 */
static void sample_to_rmt(const void *src, rmt_item32_t *dest, size_t src_size, size_t wanted_num, size_t *translated_size, size_t *item_num)
{
	
	size_t size;
	size_t num;
	uint8_t *psrc = (uint8_t *)src;
	rmt_item32_t *pdest = dest;
	for (size = 0, num = 0; size < src_size && num < wanted_num; size++, psrc++) {
		for (int bit = 0; bit < 8; bit++) {
			(pdest++)->val = firmware::rmt_bits[(*psrc & (1 << (7-bit))) != 0].val;
			num++;
		}
	}
	*translated_size = size;
	*item_num = num;
}

/** Update leds and send the animation
 *
 * This wrapper on sample_to_rmt() update animation 
 */
static void rgb_to_rmt(const void *src, rmt_item32_t *dest, size_t src_size, size_t wanted_num, size_t *translated_size, size_t *item_num)
{
	// Savagely cast to the led state
	animation::led_state *led_state = (animation::led_state*)src;
	while ((wanted_num > sizeof(firmware::color_t)*8) && (src_size >= sizeof(animation::led_state))) {
		// Update then display
		led_state->update<typeof(firmware::global_animation)>(firmware::global_animation,firmware::current_frame_ticks_forward);
		const firmware::animation_stop_t &from_stop = firmware::global_animation[led_state->current];
		const firmware::animation_stop_t &to_stop   = firmware::global_animation[from_stop.next_index];
		
		firmware::color_t ws2812_words = firmware::color_t::mix(from_stop.color,to_stop.color,led_state->remaining,from_stop.duration);
		size_t dummy_translated_size;
		sample_to_rmt(reinterpret_cast<const void*>(&ws2812_words),dest,sizeof(ws2812_words),wanted_num,&dummy_translated_size,item_num);
		// 
		*translated_size += sizeof(animation::led_state);
		src_size         -= sizeof(animation::led_state);
		wanted_num -= sizeof(firmware::color_t)*8;
		
		led_state++;
	}
}

/** Setup a strip
 *
 * This constructor setup the submited channel and gpio pin to emit WS2812 code
 * using [RMT](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html).
 */
firmware::strip::strip(rmt_channel_t channel, gpio_num_t gpio_num) : log_tag("ws2812:" + std::to_string(channel) + ":" + std::to_string(gpio_num)), TAG(log_tag.c_str()), channel(channel)
{
	// Setup log TAG
	ESP_LOGI(TAG,"Setting-up WS2812 up channel %d on GPIO %d",channel,gpio_num);
	
	// Create config
	rmt_config_t config = RMT_DEFAULT_CONFIG_TX(gpio_num, channel);
	config.clk_div = 2;
	esp_error::assert_espcode(rmt_config(&config),TAG);
	ESP_LOGD(TAG,"rmt_config()");
	
	// Install driver
	ESP_LOGD(TAG,"rmt_driver_install()");
	esp_error::assert_espcode(rmt_driver_install(channel,0,0),TAG);
	
	ESP_LOGD(TAG,"rmt_translator_init()");
	esp_error::assert_espcode(rmt_translator_init(channel,rgb_to_rmt),TAG);
	
	ESP_LOGD(TAG,"Leaving constructor");
}
/** Revert configuration
 *
 * This destructor revert configuration did on the channel. The pin is left
 * as-is.
 */
firmware::strip::~strip()
{
	ESP_LOGD(TAG,"Entering destructor");
	
	ESP_LOGD(TAG,"rmt_driver_uninstall()");
	esp_error::assert_espcode(rmt_driver_uninstall(channel),TAG);
	
	ESP_LOGI(TAG,"Removed channel %d",channel);
}
