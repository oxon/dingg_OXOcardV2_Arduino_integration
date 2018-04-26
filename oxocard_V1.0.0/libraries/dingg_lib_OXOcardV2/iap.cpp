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

 //
//  iap_https.c
//  esp32-ota-https
//
//  Updating the firmware over the air.
//
//  This module is responsible to trigger and coordinate firmware updates.
//
//  Created by Andreas Schweizer on 11.01.2017.
//  Copyright © 2017 Classy Code GmbH
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <esp_log.h>
#include <string.h>
#include "iap.h"

IAP::IAP(){ }

IAP::~IAP(){ }

int8_t IAP::init(){
	if(this->opened == 1){
		ESP_LOGE(this->LOG_TAG, "Session already open! not opnening.");
		return -1;
	}

	this->pageBufferIndex = 0;
	this->pageBuffer = (uint8_t *)malloc(IAP::PAGE_SIZE);
	if(!this->pageBuffer){
		ESP_LOGE(this->LOG_TAG, "Out of memory. IAP-Pages couldn't be allocated.");
		return -2;
	}

	if(this->findNextBootPartition() != 0 || !this->partitionToFlash){
		ESP_LOGE(this->LOG_TAG, "No partition to flash found.");
		return -3;
	}

	ESP_LOGI(this->LOG_TAG, "next boot partition is '%s'.", this->partitionToFlash->label);
	this->currentFlashAddress = this->partitionToFlash->address;

	esp_err_t result = esp_ota_begin(this->partitionToFlash, 0, &this->OTAHandle);
	if (result != ESP_OK) {
		ESP_LOGE(this->LOG_TAG, "Error while esp_ota_begin. Reson: %d.", result);
		free(this->pageBuffer);
		return -4;
	}

	ESP_LOGI(this->LOG_TAG, "iap_begin: opened IAP session for partition '%s', address 0x%08x.", this->partitionToFlash->label, this->currentFlashAddress);
	this->opened = 1;
	return 0;
}

int8_t IAP::abort(){
	ESP_LOGI(this->LOG_TAG, "aborting IAP.");
	return this->finish(0);
}

int8_t IAP::commit(){
	ESP_LOGI(this->LOG_TAG, "committing IAP.");

	int8_t result = this->writePageBuffer();
	if(result != 0){
		ESP_LOGE(this->LOG_TAG, "Commit failed in final write.");
	}

	return this->finish(1);
}

int8_t IAP::finish(uint8_t commit){
	if(!this->opened){
		ESP_LOGE(this->LOG_TAG, "session not opened.");
		return -1;
	}

	free(this->pageBuffer);
	this->pageBuffer = NULL;
	this->pageBufferIndex = 0;
	this->currentFlashAddress = 0;

	esp_err_t result = esp_ota_end(this->OTAHandle);

	if(commit){
		if(result != ESP_OK){
			ESP_LOGE(this->LOG_TAG, "Finish failed in esp_ota_end. Reason %d", result);
			return -2;
		}
		result = esp_ota_set_boot_partition(this->partitionToFlash);
		if(result != ESP_OK){
			ESP_LOGE(this->LOG_TAG, "Finish failed in esp_ota_set_boot_partition. Reason %d", result);
			return -2;
		}
	}

	this->OTAHandle = 0;
	this->partitionToFlash = NULL;
	this->opened = 0;

	return 0;
}

int8_t IAP::write(uint8_t *bytes, uint16_t length){
	ESP_LOGI(this->LOG_TAG, "write: bytes %p and len %u", bytes, length);

	if(!this->opened){
		ESP_LOGE(this->LOG_TAG, "IAP not initialized in write.");
		return -1;
	}

	while(length > 0){
		uint16_t spaceRemaining = IAP::PAGE_SIZE - this->pageBufferIndex;
		uint16_t numberOfBytesToCopy = (spaceRemaining < length) ? spaceRemaining : length;

		memcpy(&this->pageBuffer[this->pageBufferIndex], bytes, numberOfBytesToCopy);

		this->pageBufferIndex += numberOfBytesToCopy;
		bytes +=  numberOfBytesToCopy;
		length -=  numberOfBytesToCopy;

		if(this->pageBufferIndex == IAP::PAGE_SIZE){
			esp_err_t result = this->writePageBuffer();
			if (result != ESP_OK) {
				ESP_LOGE(this->LOG_TAG, "iap_write: write failed (%d)!", result);
				return -2;
			}
		}
	}

	return 0;
}

int8_t IAP::writePageBuffer(){
	ESP_LOGI(this->LOG_TAG, "Writing pageBuffer");
	if(this->pageBufferIndex == 0) return 0;

	ESP_LOGI(this->LOG_TAG, "write pageBuffer: writing %u bytes to address 0x%08x", this->pageBufferIndex, this->currentFlashAddress);
	esp_err_t result = esp_ota_write(this->OTAHandle, this->pageBuffer, this->pageBufferIndex);
	if(result != ESP_OK){
		ESP_LOGE(this->LOG_TAG, "iap_write_page_buffer: Write failed becaus of reason: %d", result);
		return -1;
	}
	this->currentFlashAddress += this->pageBufferIndex;
	this->pageBufferIndex = 0;
	return 0;
}

int8_t IAP::findNextBootPartition(){
	// Factory => app_0
	// app_0 => app_1
	// app_1 => app_0

	const esp_partition_t *currentBootPartition = esp_ota_get_boot_partition();

	if(!strcmp("factory", currentBootPartition->label)){
		this->partitionToFlash = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, "app0");
		return 0;
	}
	if(!strcmp("app0", currentBootPartition->label)){
		this->partitionToFlash = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, "app1");
		return 0;
	}
	if(!strcmp("app1", currentBootPartition->label)){
		this->partitionToFlash = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, "app0");
		return 0;
	}

	return -1;
}
