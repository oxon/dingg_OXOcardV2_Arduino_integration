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
#include "accesspointscenario.h"
#include "images.h"
#include "oxocard.h"

int8_t AccessPointScenario::execute(void){
	ESP_LOGI(this->LOG_TAG, "Running accesspoint.");
	oxocard->wifi->startAP();
	AccessPoint ap = AccessPoint();

	DNSServer s = DNSServer();
	DNSEntry eg = DNSEntry("meine.oxocard.ch", IPAddress(192,168,4,1));
	DNSEntry ee = DNSEntry("my.oxocard.ch", IPAddress(192,168,4,1));
	DNSEntry ef = DNSEntry("mon.oxocard.ch", IPAddress(192,168,4,1));
	s.addDNSEntry(eg);
	s.addDNSEntry(ee);
	s.addDNSEntry(ef);

	ap.start();
	s.start();

	while(1){
		s.processNextRequest();
		ap.handleAPConnections();
		vTaskDelay(20/portTICK_PERIOD_MS);
	}
	return 0;
}

static void handle_ap_animations_task(void *pv){
	AccessPointScenario *aps = (AccessPointScenario *)pv;
	aps->wlanIconAnimation(5);
	while(1){
		aps->oxocard->matrix->drawText(System::getName(), true, false);
		vTaskDelay(500/portTICK_PERIOD_MS);
		aps->wlanIconAnimation(2);
		if (aps->deleteAnimationsTaskRequest){
			aps->animationsHandle = NULL;
			vTaskDelete(aps->animationsHandle);
		}
	}
}

void AccessPointScenario::wlanIconAnimation(uint8_t amount){
	for(uint8_t i=0; i<amount; i++){
		this->oxocard->matrix->clear();
		this->oxocard->matrix->drawImage(imageWlanIcon, 0, 0, oxocardBrandColors[4]);
		vTaskDelay(800/portTICK_PERIOD_MS);
		this->oxocard->matrix->clearScreen();
		vTaskDelay(200/portTICK_PERIOD_MS);
	}
}

int8_t AccessPointScenario::configure(void){
	ESP_LOGI(this->LOG_TAG, "Configure accesspoint.");
	xTaskCreate(&handle_ap_animations_task, "ap_animations", 2048, (void*)this, 1, &this->animationsHandle);
	oxocard->wifi->scan();

	return 0;
}

void AccessPointScenario::deleteAnimationsTask(){
	this->deleteAnimationsTaskRequest = true;
	do {
		vTaskDelay(1/portTICK_PERIOD_MS);
	} while (this->animationsHandle != NULL);
}
