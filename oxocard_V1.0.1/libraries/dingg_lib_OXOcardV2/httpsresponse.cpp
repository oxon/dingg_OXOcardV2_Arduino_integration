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
#include "httpsresponse.h"

HTTPSResponse::HTTPSResponse(){ }

HTTPSResponse::~HTTPSResponse(){ }

int8_t HTTPSResponse::start(){
	for(int i=0; i<MAX_BUFFER_LENGTH; i++)
		this->buffer[i] = 0;

	this->bodyLength = 0;
	this->parsingBody = 0;
	this->parsingHeader = 1;
	this->currentBodyIndex = 0;
	return 0;
}

int8_t HTTPSResponse::read(size_t amount){

	size_t startPosition = 0;
	if(this->parsingHeader == 1)
		startPosition += this->handleHeader(startPosition, amount);

	if(parsingBody == 1){
		startPosition += this->handleBody(startPosition, amount-startPosition);
	}

	if(amount != startPosition){
		ESP_LOGW(this->LOG_TAG, "amount is not empty after parsing. weird? amount: %d, and current position: %d", amount, startPosition);
	}

	if(this->currentBodyIndex >= MAX_BODY_LENGTH || this->currentBodyIndex == this->bodyLength)
		return 0;
	return 1;
}

int8_t HTTPSResponse::stop(){
	ESP_LOGI(this->LOG_TAG, "stop called.");
	return 0;
}

size_t HTTPSResponse::handleHeader(size_t start, size_t amount){
	uint8_t doubleNewline = 0;
	for(int i=start; i<amount; i++){
		if(this->buffer[i] == 0 || this->buffer[i] == '\r') continue;
		if(i>0 && this->buffer[i] == '\n'  && this->buffer[i-1] == '\r'){
			if(doubleNewline == 1){
				ESP_LOGI(this->LOG_TAG, "Body incoming.");
				this->parsingBody = 1;
				this->parsingHeader = 0;
				this->prepareBody();
				return i+1;
			}
			doubleNewline = 1;
			continue;
		}

		size_t length = strlen("HTTP/1");
		if(strncmp("HTTP/1", this->buffer+i, length) == 0){
			int j = i;
			int8_t spaces = 0;
			while(this->buffer[j] != '\r'){
				if(this->buffer[j] == ' '){
					spaces++;
					j++;
					continue;
				}
				if(spaces == 1){
					this->statusCode *= 10;
					this->statusCode += this->buffer[j]-48;
				}
				j++;
			}
		}

		length = strlen("Content-Length: ");
		if(strncmp("Content-Length: ", this->buffer+i, length) == 0){
			this->hasBodyLength = 1;
			int j = i+length;
			while(this->buffer[j] != '\r'){
				this->bodyLength *= 10;
				this->bodyLength += this->buffer[j]-48;
				j++;
			}
			ESP_LOGI(this->LOG_TAG, "Content-Length is %i", this->bodyLength);
		}
		doubleNewline = 0;
	}
	return amount-start;
}

void HTTPSResponse::prepareBody(){
	this->bodyLength = (this->hasBodyLength) ? this->bodyLength : MAX_BODY_LENGTH;
	this->body = (char*)malloc(this->bodyLength+1);
	for(size_t i=0; i<this->bodyLength; i++)
		this->body[i] = '\0';
}

size_t HTTPSResponse::handleBody(size_t start, size_t amount){
	size_t i = 0;
	for(i=0; i<amount; i++){
		this->body[this->currentBodyIndex] = this->buffer[start+i];
		this->currentBodyIndex++;
	}
	return i;
}

uint16_t HTTPSResponse::getStatusCode(void){
	return this->statusCode;
}

char *HTTPSResponse::getBody(){
	return this->body;
}

char *HTTPSResponse::getBuffer(){
	return this->buffer;
}

uint32_t HTTPSResponse::getBufferLength(){
	return HTTPSResponse::MAX_BUFFER_LENGTH;
}
