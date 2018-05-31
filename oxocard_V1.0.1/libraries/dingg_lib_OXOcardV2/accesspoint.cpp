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
#include "accesspoint.h"

AccessPoint::AccessPoint(){
	this->running = 0;
	this->server = new WiFiServer();
}

AccessPoint::~AccessPoint(){
	if(this->running == 1){
		this->server->end();
		this->running = 0;
	}
}

void AccessPoint::start(void){
	this->running = 1;
	this->server->begin();
}

void AccessPoint::stop(void){
	this->server->end();
	this->running = 0;
}

void AccessPoint::handleAPConnections(){
	if(this->running){
		WiFiClient client = this->server->available();
		if(client)
			this->handleClientWithHTTP(&client);
	}
}

void AccessPoint::handleClientWithHTTP(WiFiClient *client){
	ESP_LOGI(this->LOG_TAG, "We have a client. He is connected? %i", client->connected());
	while(client->connected()){
		uint8_t buffer[2048] = {0};
		size_t bufferIndex = 0;
		while(client->available()){
			bufferIndex += client->read(buffer+bufferIndex, 2048-bufferIndex-1);
			vTaskDelay(10/portTICK_PERIOD_MS); // give em some time.
		}
		if(bufferIndex > 0){
			HTTPServer *server = new HTTPServer();
			server->parseRequest((char *)buffer, bufferIndex);
			client->write((uint8_t *)server->getResponseHeader(), server->getResponseHeaderLength());
			client->write((uint8_t *)server->getResponse(), server->getResponseLength());
			ESP_LOGI(this->LOG_TAG, "Sending header: %s", server->getResponseHeader());
			client->flush();
			client->stop();
		}else{
			vTaskDelay(50/portTICK_PERIOD_MS);
		}
	}
	client->stop();
	ESP_LOGI(this->LOG_TAG, "Client disconnected.");

}
