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
#include "httpsota.h"

HTTPSOTA::HTTPSOTA(const char *url){
	this->request = new HTTPSRequest();
	this->request->setURL(url);

}

HTTPSOTA::~HTTPSOTA(){
	this->reset();
}

void HTTPSOTA::reset(){
	if(this->iap != NULL){
		delete this->iap;
		this->iap = NULL;
	}
	if(this->tls){
		delete this->tls;
		this->tls = NULL;
	}
}

void HTTPSOTA::registerProgressCallback(void(*progressCallback)(void *), void *context){
	this->progressCallback = progressCallback;
	this->callbackContext = context;
}

int8_t HTTPSOTA::flash(){
	this->reset();

	this->iap = new IAP();
	if(this->iap->init()){
		ESP_LOGE(this->LOG_TAG, "iap init failed. stopping flashing.");
		this->reset();
		return -1;
	}

	this->tls = new WifiTLS();

	tls->setServerPort(request->getServerPort());
	tls->setServerHostname(request->getServerHostname());
	if(tls->connect() < 0){
		ESP_LOGE(this->LOG_TAG, "connect failed. stopping flashing.");
		this->reset();
		return -2;
	}

	ESP_LOGI(this->LOG_TAG, "HTTPSOTA is sending the flash request now.");

	tls->sendAndReceive((WifiTLSRequest *)request, (WifiTLSResponse *)this);

	if(this->iap->commit() < 0)
		return -3;

	return 0;
}

double HTTPSOTA::getProgress(){
	double progress = (double)this->written / (double)this->bodyLength;
	if(progress < 0.0) progress = 0.0;
	if(progress > 1.0) progress = 1.0;
	return progress;
}

void HTTPSOTA::prepareBody(){ }

size_t HTTPSOTA::handleBody(size_t start, size_t amount){

	this->iap->write((uint8_t*)this->buffer+start,amount);
	this->written += amount;

	if(this->progressCallback != NULL)
		(*this->progressCallback)(this->callbackContext);

	return amount;
}
