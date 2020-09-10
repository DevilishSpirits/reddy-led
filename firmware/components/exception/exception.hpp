#pragma once
#include <esp_err.h>
#include <esp_log.h>
#include <exception>
class esp_error: public std::exception {
	public:
		const esp_err_t code;
		const char* what() const noexcept override {
			return esp_err_to_name(code);
		}
		
		esp_error(const esp_err_t code, const char* tag) : code(code) {
			esp_log_write(ESP_LOG_ERROR,tag,what());
		};
		inline static void assert_espcode(const esp_err_t code, const char* tag) {
			if (code != ESP_OK)
			#if __cpp_exceptions
				throw esp_error(code,tag);
			#else
				esp_log_write(ESP_LOG_ERROR,tag,esp_err_to_name(code));
			#endif
		}
};
