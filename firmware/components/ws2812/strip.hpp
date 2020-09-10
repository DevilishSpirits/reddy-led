#pragma once
#include <driver/rmt.h>
#include <string>
#include <limits>
#include <exception.hpp>

namespace firmware {
	extern const rmt_item32_t rmt_bits[2];
	class strip {
		public:
			/** Tag for debugging
			 *
			 * This tag is used in debugging. It's format is "ws2812:[channel]:[pin]".
			 */
			const std::string log_tag;
			/** Tag for debugging (char* favlor)
			 *
			 * This allow nasty hack in macro expansion of ESP_LOGX.
			 */
			const char* TAG;
			
			/** The RMT channel in use
			 */
			const rmt_channel_t channel;
			
			strip(rmt_channel_t channel, gpio_num_t gpio_num);
			~strip();
			
			/** Output datas
			 * \param[in] src Datas to send. You SHOULD NOT access datas until the
			 *                transmission is done (even reading).
			 * \param src_size Size of `src`.
			 * \param wait_tx_done If true, the function will block until all datas
			 *                         have been sent.
			 *
			 * This function will send the `src` of datas on the strip setting the color
			 * of LEDs.
			 *
			 * This is a wrapper to [rmt_write_sample](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/rmt.html#_CPPv416rmt_write_sample13rmt_channel_tPK7uint8_t6size_tb).
			 */
			void write_sample(const uint8_t *src, size_t src_size, bool wait_tx_done) {
				esp_error::assert_espcode(rmt_write_sample(channel,src,src_size,wait_tx_done),TAG);
			}
			
			/** Timed wait until datas have been sent
			 * \param wait_time Maximum wait-time in ticks
			 * \return ESP_OK if transmission is done, ESP_ERR_TIMEOUT on wait_time
			 *         timeout, anything else on error.
			 *
			 * This is a wrapper to [rmt_write_sample](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/rmt.html#_CPPv416rmt_write_sample13rmt_channel_tPK7uint8_t6size_tb).
			 */
			esp_err_t wait_tx_done(TickType_t wait_time) {
				return rmt_wait_tx_done(channel,wait_time);
			}
			/** Wait until datas have been sent
			 *
			 * Unlike the timed version this version will throw on error.
			 *
			 * This is a wrapper to [rmt_write_sample](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/rmt.html#_CPPv416rmt_write_sample13rmt_channel_tPK7uint8_t6size_tb).
			 */
			void wait_tx_done() {
				esp_error::assert_espcode(rmt_wait_tx_done(channel,std::numeric_limits<TickType_t>::max()),TAG);
			}
	};
	
}
