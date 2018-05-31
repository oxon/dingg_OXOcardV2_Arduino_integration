/*
 * \license LGPL-V2.1
 * Copyright (c) 2017 OXON AG. All rights reserved.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see 'http://www.gnu.org/licenses/'
 */

#include <esp_log.h>
#include <string.h>
#include "backend.h"
#include "httpsota.h"
#include "system.h"
#include "persistentmemory.h"
#include "urlutil.h"
#include "cJSON.h"

Backend::Backend(){
	this->client = new HTTPSClient();

	PersistentMemory pm = PersistentMemory("settings");
	pm.getStr("ota_hostname", this->hostname, sizeof(this->hostname));
	pm.getStr("ota_path_prefix", this->path_prefix, sizeof(this->path_prefix));
	uint32_t port = 0;
	pm.getUInt32("ota_port", &port);
	this->port = (uint16_t)port;

	size_t len = strlen(this->hostname)+strlen(this->path_prefix)+strlen(this->protocol)+7;
	this->baseURL = (char *)malloc(len+1);
	this->baseURLLength = len;
	snprintf(this->baseURL, len, "%s%s:%d%s/", this->protocol, this->hostname, this->port, this->path_prefix);
	this->baseURL[len] = '\0';
}

Backend::~Backend(){
	delete this->client;
}

char *Backend::craftURL(const char *fmt, va_list list){
	size_t position = 0;
	size_t totalLength = vsnprintf (NULL, 0, fmt, list)+strlen(this->baseURL);
	char *url = (char *)malloc(totalLength+1);
	position += snprintf(url, totalLength, "%s", this->baseURL);
	position += vsnprintf (url+position, totalLength-position+1, fmt, list);
	return url;
}

char *Backend::craftURL(const char *fmt, ...){
	va_list list;
	va_start(list, fmt);
	char *url = this->craftURL(fmt, list);
	va_end(list);
	return url;
}

HTTPSResponse *Backend::doGETRequest(const char *fmt, ...){
	va_list list;
	va_start(list, fmt);
	char *url = this->craftURL(fmt, list);
	va_end(list);

	ESP_LOGI(this->LOG_TAG, "GET request on: '%s'", url);

	HTTPSRequest req = HTTPSRequest(url);
	HTTPSResponse *res = this->client->sendRequest(&req);
	free(url);
	return res;
}

HTTPSResponse *Backend::doPOSTRequest(const char *postBody, size_t postBodyLength, const char *fmt, ...){
	va_list list;
	va_start(list, fmt);
	char *url = this->craftURL(fmt, list);
	va_end(list);

	ESP_LOGI(this->LOG_TAG, "POST request on: '%s'", url);

	HTTPSRequest req = HTTPSRequest(url);
	req.setMethod(METHOD_POST);
	req.setPostBody(postBody, postBodyLength);
	HTTPSResponse *res = this->client->sendRequest(&req);
	free(url);
	return res;
}

int8_t Backend::flash(){
	const char *id = URLUtil::encode(System::getId());
	char *url = this->craftURL("oxocard/flash?id=%s", id);

	HTTPSOTA sota = HTTPSOTA(url);
	int8_t result = sota.flash();
	free(url);
	return result;
}

char *Backend::info(){
	const char *id = URLUtil::encode(System::getId());
	HTTPSResponse *res = this->doGETRequest("oxocard/info?id=%s", id);

	char *content = NULL;
	if(res->getStatusCode() > 199 && res->getStatusCode() < 300){
		content = res->getBody();
	}
	delete res;

	return content;
}

char *Backend::getTime(){
	HTTPSResponse *res = this->doGETRequest("getTime");

	char *content = NULL;
	if(res->getStatusCode() > 199 && res->getStatusCode() < 300){
		content = res->getBody();
	}
	delete res;

	return content;
}

char *Backend::log(char *logs, size_t length){
	const char *id = URLUtil::encode(System::getId());
	HTTPSResponse *res = this->doPOSTRequest(logs, length, "oxocard/log?id=%s", id);

	char *content = NULL;
	if(res->getStatusCode() > 199 && res->getStatusCode() < 300){
		content = res->getBody();
	}
	delete res;

	return content;
}

char *Backend::initPair(){
	const char *id = URLUtil::encode(System::getId());

	HTTPSResponse *res = this->doGETRequest("oxocard/initPairing?id=%s", id);

	char *content = NULL;
	if(res->getStatusCode() > 199 && res->getStatusCode() < 300){
		content = res->getBody();
	}
	delete res;

	return content;
}

char *Backend::registerCard(){
	cJSON *root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "id", cJSON_CreateString(System::getId()));
	cJSON_AddItemToObject(root, "name", cJSON_CreateString(System::getName()));
	char *body = cJSON_Print(root);

	HTTPSResponse *res = this->doPOSTRequest(body, strlen(body), "oxocard/register");

	cJSON_Delete(root);
	free(body);

	char *content = NULL;
	if(res->getStatusCode() > 199 && res->getStatusCode() < 300){
		content = res->getBody();
	}
	delete res;

	return content;
}

char *Backend::getWeather(const char *townid){
	const char *etownid = URLUtil::encode(townid);
	size_t len = strlen("getWeather?id=")+strlen(townid)+this->baseURLLength;
	char url[len+1] ={0};
	snprintf(url, len, "%s%s%s", this->baseURL, "getWeather?id=", etownid);
	HTTPSRequest *req = new HTTPSRequest(url);
	HTTPSResponse *res = this->client->sendRequest(req);

	char *content = res->getBody();
	delete req; delete res;

	return content;
}
