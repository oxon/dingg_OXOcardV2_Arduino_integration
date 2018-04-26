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

#include "backendlog.h"
#include "system.h"
#include "wireless.h"

BackendLog &bl = BackendLog::getInstance();

int customLogger(const char *fmt , va_list list){
	int res = bl.defaultLogger(fmt, list);
	bl.setLog(fmt, list);
	return res;
}

BackendLog::BackendLog(){
	this->backend = new Backend();
}

BackendLog::~BackendLog(){
	if(this->buffer != NULL){
		free(this->buffer);
		this->buffer = NULL;
	}
	if(this->backend != NULL){
		delete this->backend;
	}
}

void BackendLog::activate(){
	if(this->buffer == NULL)
		this->buffer = (char *)malloc(BackendLog::BUFFER_SIZE+1);
	this->defaultLogger = esp_log_set_vprintf(customLogger);

	System::printInfo();
}

void BackendLog::deactivate(){
	esp_log_set_vprintf(this->defaultLogger);
	if(this->buffer != NULL){
		free(this->buffer);
		this->buffer = NULL;
	}
}

void BackendLog::setLog(const char *fmt, va_list list){
	size_t bufferLeft = BackendLog::BUFFER_SIZE-this->bufferIndex;
	size_t totalLength = vsnprintf(this->buffer+this->bufferIndex, bufferLeft, fmt, list);

	if(totalLength>=bufferLeft){
		this->upload(this->bufferIndex);
		for(size_t i=0; i<BackendLog::BUFFER_SIZE; i++)
			this->buffer[i] = '\0';
		this->bufferIndex = 0;
		this->setLog(fmt, list);
	}else{
		this->bufferIndex += totalLength-1;
		this->buffer[this->bufferIndex] = '\n';
		this->bufferIndex++;
	}
}

void BackendLog::upload(size_t length){
	esp_log_level_t old = (esp_log_level_t)(LOG_LOCAL_LEVEL);
	esp_log_level_set("*", ESP_LOG_NONE);
	this->backend->log(this->buffer, length);
	esp_log_level_set("*", old);
}
