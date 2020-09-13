#include "web.hpp"

/** Webserver configuration
 *
 * The default one is used
 */
static httpd_config_t config = HTTPD_DEFAULT_CONFIG();
/** The webserver handle
 *
 * It's set during webserver::start()
 */
static httpd_handle_t handle = NULL;

/** @brief React on GET /infos.json
 *  @param req The request to answer
 *
 * This endpoint simply reply a JSON containing some stats about the ESP32 configuration.
 */
static esp_err_t GETinfos(httpd_req_t *req)
{
	httpd_resp_set_type(req,HTTPD_TYPE_JSON);
	return httpd_resp_sendstr(req,
	"{"
		#ifdef IDF_VER
		"\"idf_version\":\"" IDF_VER "\","
		#endif
		"\"compiler_version\":\"" __VERSION__ "\","
		"\"compile_date\":\"" __TIMESTAMP__ "\","
		#if 0 // TODO
		"\"led_pin\":" xstr(CONFIG_LED_PIN) ","
		#endif
		// TODO "\"led_count\":" xstr(CONFIG_LSTRIP_PIN+CONFIG_USTRIP_PIN) ","
		#if 0 // TODO
		"\"strips\":"
		"[{\"pin\":" xstr(CONFIG_LSTRIP_PIN) ",\"led_count\":" xstr(CONFIG_LSTRIP_LED_COUNT) "}"
		",{\"pin\":" xstr(CONFIG_USTRIP_PIN) ",\"led_count\":" xstr(CONFIG_USTRIP_LED_COUNT) "}"
		"],"
		#endif
		"\"has_white\":"
			#ifdef CONFIG_RGBW_STRIP
				"true"
			#else
				"false"
			#endif 
			","
		"\"wifi\":{"
			"\"ssid\":\"" CONFIG_WIFI_SSID "\","
			// I miss the password
			#if 0 // TODO
			"\"channel\":" xstr(CONFIG_WIFI_CHANNEL) ","
			// TODO WIFI_HIDDEN
			"\"max_connection\":" xstr(CONFIG_WIFI_MAX_CONNECTION) ","
			"\"beacon_interval\":" xstr(CONFIG_WIFI_BEACON_INTERVAL)
			#endif
		"}"
	"}");
}

const httpd_uri_t firmware::http_uri_GETinfos = {
	.uri      = "/infos.json",
	.method   = HTTP_GET,
	.handler  = GETinfos,
	.user_ctx = NULL
};

/** Start the webserver
 *
 * This configure and start the HTTP server
 */
esp_err_t firmware::start_httpd(void)
{
	esp_err_t code;
	if ((code = httpd_start(&handle,&config)) == ESP_OK) {
		httpd_register_uri_handler(handle,&http_uri_GETinfos);
		httpd_register_uri_handler(handle,&http_uri_PUTanim);
	}
	return code;
}
