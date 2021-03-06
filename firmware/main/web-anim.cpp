#include <parser.hpp>
#include "web.hpp"
#include <esp_log.h>
static const char* LOG_TAG = "web-anim";

static esp_err_t PUTanim(httpd_req_t *req)
{
	ESP_LOGI(LOG_TAG,"PUT /anim upload");
	// Feed the beast
	firmware::parser parser;
	char buffer[4096];
	int  read;
	std::string error_message;
	do {
		read = httpd_req_recv(req,buffer,sizeof(buffer));
		// Ignore EINT
		if (read == HTTPD_SOCK_ERR_TIMEOUT)
			continue;
		// Forward errors
		if (read < 0)
			return read;
		// YAJL chew
		yajl_status status = parser.parse(buffer,read);
		if (status != yajl_status_ok) {
			std::string error_message = parser.get_error(status,buffer,read);
			httpd_resp_set_type(req,"text/plain");
			httpd_resp_set_status(req,"400 Invalid JSON syntax");
			return httpd_resp_send(req,error_message.c_str(),error_message.size());
		}
	} while (read);
	// Check if YAJL liked the meat until the end
	ESP_LOGD(LOG_TAG,"Animation syntax is correct.");
	yajl_status status = parser.complete_parse();
	if (status != yajl_status_ok) {
		std::string error_message = parser.get_error(status,NULL,0);
		httpd_resp_set_type(req,"text/plain");
			httpd_resp_set_status(req,"400 Invalid JSON logic");
			return httpd_resp_send(req,error_message.c_str(),error_message.size());
	}
	// Now return sucess
	ESP_LOGI(LOG_TAG,"Animation upload successful");
	const char sucess_reply[] = "Success!";
	return httpd_resp_send(req,sucess_reply,sizeof(sucess_reply)-1);
}

const httpd_uri_t firmware::http_uri_PUTanim = {
	.uri	  = "/anim",
	.method   = HTTP_PUT,
	.handler  = PUTanim,
	.user_ctx = NULL
};
