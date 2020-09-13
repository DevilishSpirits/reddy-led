#pragma once
#include <esp_http_server.h>
namespace firmware {
	esp_err_t start_httpd(void);
	extern const httpd_uri_t http_uri_PUTanim;
	extern const httpd_uri_t http_uri_GETinfos;
}
