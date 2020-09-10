#if 0
#include <esp_system.h>
#include "config.h"
#include "animation.h"
#include "web.h"
#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_log.h>
#include <cstring>

#include <limits>
#include <atomic>
#endif
#include <animation.hpp>
#include <strip.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_task_wdt.h>

// Onboard LED helpers
static inline void board_led_set(bool state) {
	gpio_set_level(gpio_num_t(CONFIG_LED_PIN),state);
}
static inline void board_led_on(void) {
	board_led_set(true);
}
static inline void board_led_off(void) {
	board_led_set(false);
}

extern "C" {
	void app_main(void);
}

void app_main()
{
	// Setup GPIO
	gpio_pad_select_gpio(gpio_num_t(CONFIG_LED_PIN));
	gpio_set_direction(gpio_num_t(CONFIG_LED_PIN),GPIO_MODE_OUTPUT);
	// Print startup banner
	board_led_on();
	printf("RED's LED strip controller 2020-09-09\n"
	"Compiled using GCC " __VERSION__
	#ifdef IDF_VER
	", using IDF " IDF_VER
	#endif
	" on " __TIMESTAMP__ "\n\nLower strip on pin %d (%d LEDs)\nHigher strip on pin %d (%d LEDs)\n\n",CONFIG_LSTRIP_PIN,CONFIG_USTRIP_LED_COUNT,CONFIG_USTRIP_PIN,CONFIG_USTRIP_LED_COUNT);
	vTaskDelay(250 / portTICK_PERIOD_MS);
	board_led_off();
	
	// Init network
	//printf("[wifi] Initializing netif... %s\n",esp_err_to_name(esp_netif_init()));
	//printf("[wifi] Creating ESP default event loop... %s\n",esp_err_to_name(esp_event_loop_create_default()));
	// Init WiFi
	#if 0
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	wifi_init_config.nvs_enable = false;
	printf("[wifi] Initializing Wifi... %s\n",esp_err_to_name(esp_wifi_init(&wifi_init_config)));
	printf("[wifi] Setting Wifi to AP mode... %s\n",esp_err_to_name(esp_wifi_set_mode(WIFI_MODE_AP)));
	esp_netif_create_default_wifi_ap();
	wifi_config_t      wifi_config;
	strcpy((char*)wifi_config.ap.ssid,CONFIG_WIFI_SSID);
	strcpy((char*)wifi_config.ap.password,CONFIG_WIFI_PASS);
	wifi_config.ap.ssid_len = 0;
	wifi_config.ap.channel = CONFIG_WIFI_CHANNEL;
	#ifdef CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE
	wifi_config.ap.authmode = WIFI_AUTH_WPA3_PSK;
	#else
	wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
	#endif
	#ifdef CONFIG_WIFI_HIDDEN
	wifi_config.ap.ssid_hidden = 1;
	#else
	wifi_config.ap.ssid_hidden = 0;
	#endif
	wifi_config.ap.max_connection = CONFIG_WIFI_MAX_CONNECTION;
	wifi_config.ap.beacon_interval = CONFIG_WIFI_BEACON_INTERVAL;
	printf("[wifi] Setting-up Wifi... %s\n",esp_err_to_name(esp_wifi_set_config(ESP_IF_WIFI_AP,&wifi_config)));
	printf("[wifi] Starting Wifi... %s\n",esp_err_to_name(esp_wifi_start()));
	// Init the webserver
	printf("[httpd] Starting webserver... %s\n",esp_err_to_name(webserver::start(anim,front_buffer,back_buffer)));
	#endif
	// Init the strip
	firmware::strip lstrip(RMT_CHANNEL_0,gpio_num_t(CONFIG_LSTRIP_PIN));
	firmware::strip ustrip(RMT_CHANNEL_1,gpio_num_t(CONFIG_USTRIP_PIN));
	
	// Init a demo RGB wave 
	firmware::global_animation[0].color = firmware::color_t(.5,0,0);
	firmware::global_animation[0].duration   = 10;
	firmware::global_animation[0].next_index = 1;
	
	firmware::global_animation[1].color = firmware::color_t(0,0,0);
	firmware::global_animation[1].duration   = 30;
	firmware::global_animation[1].next_index = 2;
	
	firmware::global_animation[2].color = firmware::color_t(.3,.3,0);
	firmware::global_animation[2].duration   = 20;
	firmware::global_animation[2].next_index = 3;
	
	firmware::global_animation[3].color = firmware::color_t(0,0,0);
	firmware::global_animation[2].duration   = 30;
	firmware::global_animation[3].next_index = 0;
	
	for (auto &&i: firmware::lled_states) {
		i.current = 0;
		i.remaining = 0;
	}
	for (auto &&i: firmware::uled_states) {
		i.current = 0;
		i.remaining = 0;
	}
	/*for (auto i = 0; i < STRIP_LED_COUNT; i++)
		anim.led_timestamps[i] = 0;
	
	anim.buffer[0].next = 1;
	anim.buffer[0].color[COLORC_RED] = 16;
	anim.buffer[0].color[COLORC_GREEN] = 0;
	anim.buffer[0].color[COLORC_BLUE] = 0;
	anim.buffer[0].delay = 25;
	
	anim.buffer[1].next = 2;
	anim.buffer[1].color[COLORC_RED] = 16;
	anim.buffer[1].color[COLORC_GREEN] = 0;
	anim.buffer[1].color[COLORC_BLUE] = 0;
	anim.buffer[1].delay = 25;

	anim.buffer[2].next = 3;
	anim.buffer[2].color[COLORC_RED] = 0;
	anim.buffer[2].color[COLORC_GREEN] = 16;
	anim.buffer[2].color[COLORC_BLUE] = 0;
	anim.buffer[2].delay = 25;
	
	anim.buffer[3].next = 4;
	anim.buffer[3].color[COLORC_RED] = 0;
	anim.buffer[3].color[COLORC_GREEN] = 16;
	anim.buffer[3].color[COLORC_BLUE] = 0;
	anim.buffer[3].delay = 25;

	anim.buffer[4].next = 5;
	anim.buffer[4].color[COLORC_RED] = 0;
	anim.buffer[4].color[COLORC_GREEN] = 0;
	anim.buffer[4].color[COLORC_BLUE] = 16;
	anim.buffer[4].delay = 25;
	
	anim.buffer[5].next = 0;
	anim.buffer[5].color[COLORC_RED] = 0;
	anim.buffer[5].color[COLORC_GREEN] = 0;
	anim.buffer[5].color[COLORC_BLUE] = 16;
	anim.buffer[5].delay = 25;

	for (auto i = 0; i < STRIP_LED_COUNT; i++)
		anim.led_index[i] = i%6;*/
	// Enter main-loop
	printf("Entering main loop !\n");
	int64_t last_time = esp_timer_get_time();
	while (1) {
		// Reset the watchdog
		esp_task_wdt_reset();
		// Write samples and wait
		board_led_on();
		int64_t now_time = esp_timer_get_time();
		firmware::current_frame_ticks_forward = (now_time - last_time)/100000;
		lstrip.write_sample(reinterpret_cast<uint8_t*>(firmware::lled_states.data()),firmware::lled_states.size() * sizeof(firmware::lled_states[0]),false);
		ustrip.write_sample(reinterpret_cast<uint8_t*>(firmware::uled_states.data()),firmware::uled_states.size() * sizeof(firmware::uled_states[0]),true);
		lstrip.wait_tx_done(std::numeric_limits<TickType_t>::max());
		board_led_off();
		last_time = now_time;
	}
}
