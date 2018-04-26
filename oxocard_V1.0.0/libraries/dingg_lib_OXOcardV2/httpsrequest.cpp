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
#include "httpsrequest.h"

HTTPSRequest::HTTPSRequest(){

}

HTTPSRequest::~HTTPSRequest(){
	if(this->buffer != NULL){
		free(this->buffer);
		this->buffer = NULL;
	}
}

HTTPSRequest::HTTPSRequest(const char *url){
	this->setURL(url);
}

int8_t HTTPSRequest::setURL(const char *url){
	this->url = url;
	return this->parseURL();
}

int8_t HTTPSRequest::setMethod(HTTPMethod method){
	this->method = method;
	this->parseURL();
	return 0;
}

int8_t HTTPSRequest::reset(){
	if(this->buffer != NULL){
		free(this->buffer);
		this->buffer = NULL;
	}
	this->port = 0;

	for(int i=0; i<HTTPSRequest::MAX_HOSTNAME_LENGTH; i++)
		this->hostname[i] = 0;
	for(int i=0; i<HTTPSRequest::MAX_PATH_LENGTH; i++)
		this->path[i] = 0;
	return 0;
}

int8_t HTTPSRequest::parseURL(){
	int foundProtocol = 0;
	if(strncmp("http", this->url, 4) == 0){
		if(this->url[4] != 's'){
			ESP_LOGW(this->LOG_TAG, "Invalid Protocl in URL.");
		}
		foundProtocol = 1;
	}

	this->reset();

	int parsingHostname = 1;
	int parsingPort = 0;
	int parsingPath = 0;
	int currentIndex = 0;
	uint16_t i = (foundProtocol) ? 8 : 0;
	while(this->url[i] != 0){
		if(parsingHostname && this->url[i] != ':' && this->url[i] != '/'){
			if(currentIndex > MAX_HOSTNAME_LENGTH){
			//error
			}
			this->hostname[currentIndex] = this->url[i];
			currentIndex++;
		}else if(parsingHostname){
			parsingHostname = 0;
			if(url[i] == ':')
				parsingPort = 1;
			else
				parsingPath = 1;
			i++;
			this->hostnameLength = currentIndex;
			currentIndex = 0;
			continue;
		}

		if(parsingPort && this->url[i] != '/'){
			this->port *= 10;
			this->port += this->url[i]-48;
		}else if(parsingPort){
			parsingPort = 0;
			parsingPath = 1;
			currentIndex = 0;
			i++;
			continue;
		}

		if(parsingPath){
			if(currentIndex == 0){
				this->path[0] = '/';
				currentIndex++;
			}
			this->path[currentIndex] = this->url[i];
			currentIndex++;
			this->pathLength = currentIndex;
		}

		i++;
	}

	if(this->buffer != NULL) free(this->buffer);

	this->bufferLength = this->pathLength + this->hostnameLength + 8 + 8 + 5 + 53 + 10; // method + newlins + length + headers chars + reserve
	this->bufferLength += this->bodyLength;

	this->buffer = (char *)malloc(this->bufferLength+1);

	size_t len = snprintf(this->getBuffer(), this->getBufferLength(), "%s %s HTTP/1.0\r\n", HTTPMethodNames[this->method], this->path);
	len += snprintf(this->getBuffer()+len, this->getBufferLength()-len, "Host: %s\r\nUser-Agent: oxocard\r\n", this->hostname);
	len += snprintf(this->getBuffer()+len, this->getBufferLength()-len, "Content-Length: %d\r\n\r\n", this->bodyLength);

	if(this->method == METHOD_POST){
		strncpy(this->buffer+len, this->body, this->bodyLength);
	}

	//ESP_LOGI(this->LOG_TAG, "the request: %s", this->getBuffer());

	return 0;
}

void HTTPSRequest::setPostBody(const char *body, size_t length){
	this->body = body;
	this->bodyLength = length;
	this->parseURL();
}

char *HTTPSRequest::getServerHostname(){
	return this->hostname;
}

uint16_t HTTPSRequest::getServerPort(){
	if(this->port == 0){
		ESP_LOGW(this->LOG_TAG, "No port was provided, so we fallback to 443.");
		return 443;
	}
	return this->port;
}

char *HTTPSRequest::getPath(){
	return this->path;
}

char *HTTPSRequest::getBuffer(){
	return this->buffer;
}

size_t HTTPSRequest::getBufferLength(){
	return this->bufferLength;
}
