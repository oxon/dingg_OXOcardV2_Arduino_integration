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
#include "wifitlsrequest.h"
/*
WifiTLSRequest::~WifiTLSRequest(){
	if(this->buffer)
		free(this->buffer);
}

char *WifiTLSRequest::getBuffer(){
	if(this->buffer == NULL){
		this->buffer = (char*)malloc(this->getBufferLength());
	}
	if(!this->buffer)
		ESP_LOGE(this->LOG_TAG, "We couldn't allocate buffer.");
	return this->buffer;
}

void WifiTLSRequest::resetBuffer(){

}

size_t WifiTLSRequest::getBufferLength(){
	return 4096;
}

size_t WifiTLSRequest::getBufferContentLength(){
	return strnlen(this->buffer, this->getBufferLength());
}*/
