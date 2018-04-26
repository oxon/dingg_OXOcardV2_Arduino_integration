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
#include "images.h"
#include "oxocard.h"
#include "pairingscenario.h"
#include "statistics.h"
#include "cJSON.h"
#include "backend.h"

int8_t PairingScenario::execute(void){
	ESP_LOGI(this->LOG_TAG, "Running pairing.");

	Statistics s = Statistics();
	Backend b = Backend();

	char *content = b.registerCard();
	if(content == NULL){
		ESP_LOGE(this->LOG_TAG, "Couldn't request register.");
		return -1;
	}
	free(content);

	content = b.initPair();
	if(content == NULL){
		ESP_LOGE(this->LOG_TAG, "Couldn't init pairing.");
		return -2;
	}

	cJSON *root = cJSON_Parse(content);
	free(content);
	cJSON *challenge = cJSON_GetObjectItemCaseSensitive(root, "challenge");

	if(!cJSON_IsString(challenge) || challenge->valuestring == NULL){
		ESP_LOGE(this->LOG_TAG, "Wrong json received from server? We can't parse challenge.");
		return -3;
	}

	ESP_LOGI(this->LOG_TAG, "We have challenge: %s", challenge->valuestring);
	vTaskDelete(this->animationsHandle);

	if(this->parseAndDrawChallenge(challenge->valuestring)){
		ESP_LOGE(this->LOG_TAG, "We cant't draw challenge.");
		return -4;
	}

	cJSON_Delete(root);

	vTaskDelay(1000/portTICK_PERIOD_MS);

	uint8_t maxWait = 60;
	uint8_t challengeIsGone = 0;
	while(challengeIsGone == 0 && maxWait > 0){
		content = b.info();
		root = cJSON_Parse(content);
		if(content == NULL){
			ESP_LOGW(this->LOG_TAG, "the info response from server was NULL");
			vTaskDelay(1000/portTICK_PERIOD_MS);
			continue;
		}
		//ESP_LOGI(this->LOG_TAG, "We have respone %s", content);
		cJSON *challengeTime = cJSON_GetObjectItemCaseSensitive(root, "challengeTime");
		if(!cJSON_IsNumber(challengeTime) || challengeTime->valuedouble == 0){
			challengeIsGone = 1;
		}else{
			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
		cJSON_Delete(root);
		free(content);
		maxWait --;
	}

	if(challengeIsGone == 0){
		ESP_LOGW(this->LOG_TAG, "Pairing failed");
		s.increaseCounter("pairing_failed");
		this->oxocard->matrix->clear();
		this->oxocard->matrix->drawImage(imageXIcon, 0, 0, rgb(255,0,0));
	}else{
		ESP_LOGI(this->LOG_TAG, "Done pairing. rebooting in 1500ms.");
		s.increaseCounter("pairing_success");
		this->oxocard->matrix->clear();
		this->oxocard->matrix->drawImage(imageCheckmarkIcon, 0, 0, rgb(0,255,0));
		this->oxocard->matrix->clear();
		vTaskDelay(1500/portTICK_PERIOD_MS);

		oxocard->system->reboot();
	}

	delay(30000);
	return 0;
}

static void handle_animations(void *pv){
	PairingScenario *ps = (PairingScenario *)pv;
	ps->wlanIconAnimation();
}


void PairingScenario::wlanIconAnimation(){
	this->oxocard->matrix->setForeColor(oxocardBrandColors[1]);
	this->oxocard->matrix->clear();
	this->oxocard->matrix->drawImage(imageConnectIcon, 0, 0, oxocardBrandColors[3]);
	vTaskDelay(5000/portTICK_PERIOD_MS);
	while(1){
		this->oxocard->matrix->clear();
		this->oxocard->matrix->drawImage(
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00011000,
			0b00011000,
			0b00000000);
		vTaskDelay(300/portTICK_PERIOD_MS);
		this->oxocard->matrix->drawImage(
			0b00000000,
			0b00000000,
			0b00000000,
			0b00111100,
			0b01000010,
			0b00011000,
			0b00011000,
			0b00000000
		);
		vTaskDelay(300/portTICK_PERIOD_MS);
		this->oxocard->matrix->drawImage(
			0b00000000,
			0b01111110,
			0b10000001,
			0b00111100,
			0b01000010,
			0b00011000,
			0b00011000,
			0b00000000
		);
		vTaskDelay(300/portTICK_PERIOD_MS);
	}
}

int8_t PairingScenario::configure(void){

	ESP_LOGI(this->LOG_TAG, "Configure pairing.");
	xTaskCreate(&handle_animations, "pair_animations", 2048, (void*)this, 1, &this->animationsHandle);
	oxocard->wifi->autoConnect();
	if(!oxocard->wifi->isConnected()){
		vTaskDelete(this->animationsHandle);
		for(uint8_t i=0; i<20; i++){
			this->oxocard->matrix->clear();
			this->oxocard->matrix->drawImage(imageWlanIcon, 0, 0, oxocardBrandColors[3]);
			vTaskDelay(200/portTICK_PERIOD_MS);
			this->oxocard->matrix->clear();
			this->oxocard->matrix->update();
			vTaskDelay(200/portTICK_PERIOD_MS);
		}
		return -1;
	}
	return 0;
}

int8_t PairingScenario::parseAndDrawChallenge(const char *challengeString){

	if(strlen(challengeString) != 5){
		ESP_LOGE(this->LOG_TAG, "Challenge string not correct length.")
		return -1;
	}

	if(challengeString[0] != 'x'){
		//TODO See log
		ESP_LOGW(this->LOG_TAG, "Challenge string has 5 fields. Needs to be implemented");
	}

	int challenge[5] = {0};
	for(size_t i=0; i<5; i++)
		challenge[i] = challengeString[i]-48;

	this->oxocard->matrix->disableAutoUpdate();
	this->oxocard->matrix->fill();
	this->oxocard->matrix->drawRectangle(0,0,4,4,this->challengeColorMap[challenge[1]]);
	this->oxocard->matrix->drawRectangle(4,0,4,4,this->challengeColorMap[challenge[2]]);
	this->oxocard->matrix->drawRectangle(0,4,4,4,this->challengeColorMap[challenge[3]]);
	this->oxocard->matrix->drawRectangle(4,4,4,4,this->challengeColorMap[challenge[4]]);
	this->oxocard->matrix->update();
	this->oxocard->matrix->noFill();
	this->oxocard->matrix->enableAutoUpdate();
	this->oxocard->matrix->clear();
	return 0;
}
