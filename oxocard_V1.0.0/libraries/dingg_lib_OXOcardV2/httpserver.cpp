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
#include <list>
#include <nvs_flash.h>
#include <string.h>
#include <wificonfiguration.h>
#include "debug_html.h"
#include "httpserver.h"
#include "index_html.h"
#include "system.h"
#include "cJSON.h"

char error_page[] = "<h1>Not found.</h1>";

HTTPServer::HTTPServer(){ }

HTTPServer::~HTTPServer(){ }

void HTTPServer::parseRequest(const char *buffer, size_t length){
	size_t lastStart = 0;
	size_t i = 0;

	this->parsingHeader = 1;
	while(i<length){

		if(this->parsingHeader && buffer[i] == '\n' && i > 0 && buffer[i-1] == '\r'){
			this->parseHeaderLine(buffer+lastStart, i-1-lastStart);
			lastStart = i+1;
		}
		if(parsingHeader == 0 && this->method == METHOD_POST){
			ESP_LOGI(this->LOG_TAG, "Body length for parsing: %i", this->postDataLength);
			this->parseBody(buffer+i+1, this->postDataLength);
			break;
		}

		i++;
	}
	this->processRequest();
}

void HTTPServer::parseHeaderLine(const char *buffer, size_t length){
	if(length == 0){
		this->parsingHeader = 0;
	}

	if(strncmp(buffer, "GET /", 5) == 0 || strncmp(buffer, "POST /", 6) == 0){

		if(buffer[0] == 'G'){
			ESP_LOGI(this->LOG_TAG, "we have a GET request.");
			this->method = METHOD_GET;
		}else if(buffer[0] == 'P'){
			ESP_LOGI(this->LOG_TAG, "we have a POST request.");
			this->method = METHOD_POST;
		}
		uint8_t pathStart=0;
		uint8_t index = 0;
		while(index<length){
			if(buffer[index] == ' ' && pathStart == 0){
				pathStart = index+1;
			}
			else if(buffer[index] == ' ' && pathStart > 0){
				strncpy(this->requestPath, buffer+pathStart, index-pathStart);
				break;
			}
			index++;
		}
		ESP_LOGI(this->LOG_TAG, "We have path: >%s<", this->requestPath);
	}

	if(strncmp(buffer, "Content-Length: ", 16) == 0){
		for(uint8_t i=16; i<length; i++){
			if(buffer[i] < 48 && buffer[i] > 57) continue;
			this->postDataLength *= 10;
			this->postDataLength += buffer[i]-48;
		}
	}
}

void HTTPServer::parseBody(const char *buffer, size_t length){
	this->postData = (char*)malloc(length+1);
	for(int i=0; i<length; i++)
		this->postData[i] = buffer[i];
	this->postData[length] = '\0';
}

void HTTPServer::processRequest(){
	ESP_LOGI(this->LOG_TAG, "Processing request.");
	if(this->method == METHOD_GET){
		ESP_LOGI(this->LOG_TAG, "We have length of: %i", strlen(this->requestPath));
		if( strncmp(this->requestPath, "/", 1) == 0 && strlen(this->requestPath) == 1 )
			this->serveIndex();
		else if( strncmp(this->requestPath, "/index.html", 11) == 0 )
			this->serveIndex();
		else if( strncmp(this->requestPath, "/wifilist",9) == 0 )
			this->serveWifiList();
		else if( strncmp(this->requestPath, "/info", 5) == 0 )
			this->serveInfo();
		else if( strncmp(this->requestPath, "/debug.html", 11) == 0 )
			this->serveDebug();
		else if( strncmp(this->requestPath, "/debug", 6) == 0 )
			this->serveDebug();
		else if( strncmp(this->requestPath, "/factoryreset", 13) == 0 )
			this->serveFactoryReset();
		else
			this->serveError();
	}
	if(this->method == METHOD_POST){
		if( strncmp(this->requestPath, "/delete", 7) == 0 ){
			this->serveDelete();
		}else if( strncmp(this->requestPath, "/connectivity", 13) == 0 ){
			this->serveConnectivity();
		}else if( strncmp(this->requestPath, "/savedebug", 10) == 0){
			this->serveSaveDebug();
		}else if( strncmp(this->requestPath, "/save", 5) == 0){
			this->serveSave();
		}else{
			this->serveError();
		}
	}
}

void HTTPServer::serveSave(){
	cJSON *parse = cJSON_Parse(this->postData);

	uint8_t storedName = 0;
	cJSON *name = cJSON_GetObjectItemCaseSensitive(parse, "oxocard_name");
	if (cJSON_IsString(name) && (name->valuestring != NULL)){
		ESP_LOGI(this->LOG_TAG, "We're storing name: >%s<", name->valuestring);
		if(System::setName(name->valuestring)==0)
			storedName = 1;
	}

	char *toStoreWifiName = NULL;
	char *toStoreWifiPassword = NULL;
	cJSON *wifiList = cJSON_GetObjectItemCaseSensitive(parse, "wifi_name_list");
	cJSON *wifiManual = cJSON_GetObjectItemCaseSensitive(parse, "wifi_name_manual");
	if (cJSON_IsString(wifiList) && (wifiList->valuestring != NULL)){
		toStoreWifiName = wifiList->valuestring;
	}else if(cJSON_IsString(wifiManual) && (wifiManual->valuestring != NULL)){
		toStoreWifiName = wifiManual->valuestring;
	}

	cJSON *wifiPassword = cJSON_GetObjectItemCaseSensitive(parse, "wifi_password");
	if (cJSON_IsString(wifiPassword) && (wifiPassword->valuestring != NULL)){
		toStoreWifiPassword = wifiPassword->valuestring;
	}

	uint8_t storedWifi = 0;
	if(toStoreWifiName != NULL && toStoreWifiPassword != NULL){
		ESP_LOGI(this->LOG_TAG, "We have Wifi to store.");
		if(Wireless::storeWifi(toStoreWifiName, toStoreWifiPassword) == 0)
			storedWifi = 1;
	}else{
		ESP_LOGW(this->LOG_TAG, "No Wifi has been found to save.");
	}

	cJSON_Delete(parse);

	cJSON *root = cJSON_CreateObject();
	if(storedWifi == 1 || storedName == 1){
		cJSON_AddItemToObject(root, "success", cJSON_CreateTrue());
	}else{
		cJSON_AddItemToObject(root, "success", cJSON_CreateFalse());
	}
	cJSON_AddItemToObject(root, "name", cJSON_CreateString(System::getName()));
	cJSON_AddItemToObject(root, "id", cJSON_CreateString(System::getId()));

	this->response = cJSON_PrintUnformatted(root);
	this->responseLength = 0;
	for(size_t i=0; this->response[i] != 0; i++)
		this->responseLength++;
	this->contentType = (char*)"application/json";
	this->craftHeader();
	cJSON_Delete(root);
}

void HTTPServer::serveSaveDebug(){
	PersistentMemory pm = PersistentMemory("settings", PersistentMemory::WRITE_MODE);

	cJSON *parse = cJSON_Parse(this->postData);

	uint8_t hostnameStored = 0;
	cJSON *otaHostname = cJSON_GetObjectItemCaseSensitive(parse, "ota_hostname");
	if (cJSON_IsString(otaHostname) && (otaHostname->valuestring != NULL)){
		if(pm.saveStr("ota_hostname", otaHostname->valuestring) == 0)
			hostnameStored = 1;
		ESP_LOGI(this->LOG_TAG, "Storing otaHostname: >%s< result: %d", otaHostname->valuestring, hostnameStored);
	}

	uint8_t pathPrefixStored = 0;
	cJSON *otaPathPrefix = cJSON_GetObjectItemCaseSensitive(parse, "ota_path_prefix");
	if (cJSON_IsString(otaPathPrefix) && (otaPathPrefix->valuestring != NULL)){
		if(pm.saveStr("ota_path_prefix", otaPathPrefix->valuestring) == 0)
			pathPrefixStored = 1;
		ESP_LOGI(this->LOG_TAG, "Storing otaPathPrefix: >%s< result: %d", otaPathPrefix->valuestring, pathPrefixStored);
	}


	uint8_t portStored = 0;
	cJSON *otaPort = cJSON_GetObjectItemCaseSensitive(parse, "ota_port");
	if (cJSON_IsNumber(otaPort) && (otaPort->valuedouble != 0)){
		uint16_t portValue = (uint16_t)otaPort->valuedouble;
		if(pm.saveUInt32("ota_port", portValue) == 0)
			portStored = 1;
		ESP_LOGI(this->LOG_TAG, "Storing otaPort: >%d< result: %d", portValue, portStored);
	}

	cJSON_Delete(parse);

	cJSON *root = cJSON_CreateObject();

	cJSON_AddItemToObject(root, "ota_hostname",
		(hostnameStored ? cJSON_CreateTrue() : cJSON_CreateFalse())
	);
	cJSON_AddItemToObject(root, "ota_path_prefix",
		(pathPrefixStored ? cJSON_CreateTrue() : cJSON_CreateFalse())
	);
	cJSON_AddItemToObject(root, "ota_port",
		(portStored ? cJSON_CreateTrue() : cJSON_CreateFalse())
	);
	cJSON_AddItemToObject(root, "name", cJSON_CreateString(System::getName()));
	cJSON_AddItemToObject(root, "id", cJSON_CreateString(System::getId()));

	this->response = cJSON_PrintUnformatted(root);
	this->responseLength = 0;
	for(size_t i=0; this->response[i] != 0 && i<4096; i++)
		this->responseLength++;
	this->contentType = (char*)"application/json";
	this->craftHeader();
	cJSON_Delete(root);
}

void HTTPServer::serveDelete(){
	cJSON *parse = cJSON_Parse(this->postData);
	ESP_LOGI(this->LOG_TAG, "delete-post: >%s<", this->postData);
	uint8_t deletedWifi = 0;
	cJSON *index = cJSON_GetObjectItemCaseSensitive(parse, "wifiIndex");
	if (cJSON_IsNumber(index)){
		uint8_t wifiIndex = (uint8_t)index->valuedouble;
		ESP_LOGI(this->LOG_TAG, "We should delete wifi: >%d<", wifiIndex);
		if(Wireless::deleteWifi(wifiIndex) == 0)
			deletedWifi = 1;
	}

	cJSON_Delete(parse);

	cJSON *root = cJSON_CreateObject();
	if(deletedWifi == 1){
		cJSON_AddItemToObject(root, "success", cJSON_CreateTrue());
	}else{
		cJSON_AddItemToObject(root, "success", cJSON_CreateFalse());
	}

	this->response = cJSON_PrintUnformatted(root);
	this->responseLength = 0;
	for(size_t i=0; this->response[i] != 0; i++)
		this->responseLength++;
	this->contentType = (char*)"application/json";
	this->craftHeader();
	cJSON_Delete(root);
}

void HTTPServer::serveConnectivity(){
	cJSON *parse = cJSON_Parse(this->postData);

	cJSON *ssid = cJSON_GetObjectItemCaseSensitive(parse, "ssid");
	cJSON *password = cJSON_GetObjectItemCaseSensitive(parse, "password");

	WifiConfiguration wc = WifiConfiguration();
	uint8_t validInput = 1;
	if(ssid->valuestring != NULL)
		snprintf(wc.ssid, 33, "%s", ssid->valuestring);
	else
		validInput = 0;
	if(password->valuestring != NULL)
		snprintf(wc.password, 33, "%s", password->valuestring);
	else
		validInput = 0;

	cJSON_Delete(parse);

	cJSON *root = cJSON_CreateObject();
	if(validInput == 1 && Wireless::testConnectivity(wc) == 0){
		cJSON_AddItemToObject(root, "success", cJSON_CreateTrue());
	}else{
		cJSON_AddItemToObject(root, "success", cJSON_CreateFalse());
	}

	this->response = cJSON_PrintUnformatted(root);
	this->responseLength = 0;
	for(size_t i=0; this->response[i] != 0; i++)
		this->responseLength++;
	this->contentType = (char*)"application/json";
	this->craftHeader();
	cJSON_Delete(root);
}

void HTTPServer::serveIndex(){
	this->response = (char*)index_html;
	this->responseLength = index_html_len;
	this->contentType = (char*)"text/html";
	this->craftHeader();
}

void HTTPServer::serveDebug(){
	this->response = (char*)debug_html;
	this->responseLength = debug_html_len;
	this->contentType = (char*)"text/html";
	this->craftHeader();
}

void HTTPServer::serveError(){
	this->response = (char*)error_page;
	this->responseLength = strlen(error_page);
	this->statusCode = 404;
	this->contentType = (char*)"text/html";
	this->craftHeader();
}

void HTTPServer::serveWifiList(){
	cJSON *stored = cJSON_CreateArray();
	cJSON *available = cJSON_CreateArray();
	std::list<WifiConfiguration>::iterator ait = Wireless::availableWifis.begin();
	while(ait != Wireless::availableWifis.end()){
		cJSON_AddItemToArray(available, cJSON_CreateString(((WifiConfiguration)*ait).ssid));
		ait++;
	}
	std::list<WifiConfiguration>::iterator sit = Wireless::storedWifis.begin();
	while(sit != Wireless::storedWifis.end()){
		cJSON_AddItemToArray(stored, cJSON_CreateString(((WifiConfiguration)*sit).ssid));
		sit++;
	}
	cJSON *root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "availableWifis", available);
	cJSON_AddItemToObject(root, "storedWifis", stored);
	this->response = cJSON_Print(root);
	this->responseLength = 0;
	for(size_t i=0; this->response[i] != 0; i++)
		this->responseLength++;
	this->contentType = (char*)"application/json";
	this->craftHeader();
	cJSON_Delete(root);
}

void HTTPServer::serveInfo(){
	cJSON *root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "name", cJSON_CreateString(System::getName()));
	cJSON_AddItemToObject(root, "id", cJSON_CreateString(System::getId()));

	PersistentMemory pm = PersistentMemory("settings");

	char value[128] = {0};
	if(pm.getStr("ota_hostname", (char *)&value, sizeof(value)) != 0){
		cJSON_AddItemToObject(root, "ota_hostname", cJSON_CreateString(value));
	}else{
		cJSON_AddItemToObject(root, "ota_hostname", cJSON_CreateNull());
	}
	if(pm.getStr("ota_path_prefix", (char *)&value, sizeof(value)) != 0){
		cJSON_AddItemToObject(root, "ota_path_prefix", cJSON_CreateString(value));
	}else{
		cJSON_AddItemToObject(root, "ota_path_prefix", cJSON_CreateNull());
	}
	uint32_t valueint = 0;
	if(pm.getUInt32("ota_port", &valueint) != 0){
		cJSON_AddItemToObject(root, "ota_port", cJSON_CreateNumber(valueint));
	}

	this->response = cJSON_Print(root);
	this->responseLength = 0;
	for(size_t i=0; this->response[i] != 0; i++)
		this->responseLength++;
	this->contentType = (char*)"application/json";
	this->craftHeader();
	cJSON_Delete(root);
}

// This method is super ugly. Really edge-case and concept of System-Class was
// misunderstood so I now need to hack if I don't want to refactor a lot.
void HTTPServer::serveFactoryReset(){
	ESP_LOGW("factoryReset", "Erasing default NVS partition => This Oxocard is to be rebo(rn/otet).");
	nvs_flash_erase();
	esp_restart();
}

void HTTPServer::craftHeader(){
	this->responseHeader = (char*)malloc(120);
	this->responseHeaderLength = snprintf(this->responseHeader, 130, "HTTP/1.0 %d OK\r\nContent-type: %s\r\nContent-Length: %i\r\nAccess-Control-Allow-Origin: *\r\n\r\n", this->statusCode, this->contentType, this->responseLength);
}

char *HTTPServer::getResponseHeader(){
	return this->responseHeader;
}

size_t HTTPServer::getResponseHeaderLength(){
	return this->responseHeaderLength;

}

char *HTTPServer::getResponse(){
	return this->response;
}

size_t HTTPServer::getResponseLength(){
	return this->responseLength;
	//return 110;
}
