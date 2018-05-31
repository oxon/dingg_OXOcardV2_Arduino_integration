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
#include "persistentmemory.h"

PersistentMemory::PersistentMemory(const char *identifier){
	this->constructor(identifier, 0);
}

PersistentMemory::PersistentMemory(const char *identifier, uint8_t writeMode){
	this->constructor(identifier, writeMode);
}

void PersistentMemory::constructor(const char *identifier, uint8_t writeMode){
	ESP_LOGI(this->LOG_TAG, "PersistentMemory constructor with identifier: %s. writing? %d", identifier, writeMode);
	this->identifier = identifier;
	this->writeMode = writeMode;
	this->open();
}

PersistentMemory::~PersistentMemory(){
	this->close();
}

const char *PersistentMemory::get_error(esp_err_t err){
	if(err > ESP_ERR_NVS_BASE)
		return this->nvs_errors[(err)&~(ESP_ERR_NVS_BASE)];
	return this->nvs_errors[0];
}


int8_t PersistentMemory::open(){
	if(this->initialized == 1){
		return 0;
	}
	if(!this->identifier){
		ESP_LOGW(this->LOG_TAG, "No identifier provided for opening.");
		return -1;
	}
	esp_err_t err;
	if(this->writeMode == 1){
		err = nvs_open(this->identifier, NVS_READWRITE, &this->nvsHandle);
	}else{
		err = nvs_open(this->identifier, NVS_READONLY, &this->nvsHandle);
	}
	if(err != 0){
		ESP_LOGE(this->LOG_TAG, "We couldn't open the NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	ESP_LOGI(this->LOG_TAG, "NVS initialized for ID: %s", this->identifier);
	this->initialized = 1;
	return 0;
}

int8_t PersistentMemory::close(){
	if(this->initialized == 0){
		return 0;
	}
	nvs_close(this->nvsHandle);
	this->initialized = 0;
	return 0;
}

int8_t PersistentMemory::clear(){
	if(this->initialized == 0 || this->writeMode != 1){
		ESP_LOGW(this->LOG_TAG, "Trying to clear memory without opened or writeMode.");
		return -1;
	}
	esp_err_t err = nvs_erase_key(this->nvsHandle, this->identifier);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't clear the NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	return 0;
}

int8_t PersistentMemory::remove(const char *key){
	if(this->initialized == 0 || this->writeMode == 0 || !key){
		ESP_LOGW(this->LOG_TAG, "Trying to delete memory without key, opened or writeMode.");
		return -1;
	}
	esp_err_t err = nvs_erase_key(this->nvsHandle, key);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't delete from the NVS. ID: %s. Key: %s. Error: %s", this->identifier, key, this->get_error(err));
		return -2;
	}
	return 0;
}

int8_t PersistentMemory::saveByte(const char *key, uint8_t value){
	if(this->initialized == 0 || this->writeMode == 0 || !key){
		ESP_LOGW(this->LOG_TAG, "Trying to save to memory without key, opened or writeMode.");
		return -1;
	}
	esp_err_t err = nvs_set_u8(this->nvsHandle, key, value);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't save to NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	err = nvs_commit(this->nvsHandle);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't clear the NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -3;
	}
	return 0;
}

int8_t PersistentMemory::saveInt32(const char *key, int32_t value){
	if(this->initialized == 0 || this->writeMode == 0 || !key){
		ESP_LOGW(this->LOG_TAG, "Trying to save to memory without key, opened or writeMode.");
		return -1;
	}
	esp_err_t err = nvs_set_i32(this->nvsHandle, key, value);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't save to NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	err = nvs_commit(this->nvsHandle);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't clear the NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -3;
	}
	return 0;
}

int8_t PersistentMemory::saveUInt32(const char *key, uint32_t value){
	if(this->initialized == 0 || this->writeMode == 0 || !key){
		ESP_LOGW(this->LOG_TAG, "Trying to save to memory without key, opened or writeMode.");
		return -1;
	}
	esp_err_t err = nvs_set_u32(this->nvsHandle, key, value);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't save to NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	err = nvs_commit(this->nvsHandle);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't clear the NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -3;
	}
	return 0;
}

int8_t PersistentMemory::saveStr(const char *key, const char *value){
	if(this->initialized == 0 || this->writeMode == 0 || !key){
		ESP_LOGW(this->LOG_TAG, "Trying to save to memory without key, opened or writeMode.");
		return -1;
	}
	esp_err_t err = nvs_set_str(this->nvsHandle, key, value);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't save to NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	err = nvs_commit(this->nvsHandle);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't clear the NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -3;
	}
	return 0;
}

int8_t PersistentMemory::saveBlob(const char *key, const void *value, size_t len){
	if(this->initialized == 0 || this->writeMode == 0 || !key){
		ESP_LOGW(this->LOG_TAG, "Trying to save to memory without key, opened or writeMode.");
		return -1;
	}
	esp_err_t err = nvs_set_blob(this->nvsHandle, key, value, len);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't save to NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	err = nvs_commit(this->nvsHandle);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't clear the NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -3;
	}
	return 0;
}

int8_t PersistentMemory::getByte(const char *key, uint8_t *value){
	if(this->initialized == 0 || !value || !key){

		ESP_LOGW(this->LOG_TAG, "Trying to fetch from memory without key, opened or value.");
		return -1;
	}
	esp_err_t err = nvs_get_u8(this->nvsHandle, key, value);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't fetch from NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	return 0;
}

int8_t PersistentMemory::getInt32(const char *key, int32_t *value){
	if(this->initialized == 0 || !value || !key){

		ESP_LOGW(this->LOG_TAG, "Trying to fetch from memory without key, opened or value.");
		return -1;
	}
	esp_err_t err = nvs_get_i32(this->nvsHandle, key, value);

	if(err == ESP_ERR_NVS_NOT_FOUND){
		ESP_LOGW(this->LOG_TAG, "Couldn't find key: %s", key);
		return 0;
	}


	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't fetch from NVS. ID: %s. Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	return 1;
}

int8_t PersistentMemory::getUInt32(const char *key, uint32_t *value){
	if(this->initialized == 0 || !value || !key){

		ESP_LOGW(this->LOG_TAG, "Trying to fetch from memory without key, opened or value.");
		return -1;
	}
	esp_err_t err = nvs_get_u32(this->nvsHandle, key, value);

	if(err == ESP_ERR_NVS_NOT_FOUND){
		ESP_LOGW(this->LOG_TAG, "Couldn't find key: %s", key);
		return 0;
	}

	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't fetch from NVS. ID: %s | Error: %s", this->identifier, this->get_error(err));
		return -2;
	}
	return 1;
}

int16_t PersistentMemory::getStr(const char *key, char *value, size_t maxLength){
	if(this->initialized == 0 || !value || !key || maxLength == 0){
		ESP_LOGI(this->LOG_TAG, "init: %d, value: %s, key: %s, maxLength: %u", this->initialized, value, key, maxLength);
		ESP_LOGW(this->LOG_TAG, "Trying to fetch from memory without key, opened or len.");
		return -1;
	}
	size_t len = 0;
	esp_err_t err = nvs_get_str(this->nvsHandle, key, NULL, &len);
	if(err == ESP_ERR_NVS_NOT_FOUND){
		ESP_LOGW(this->LOG_TAG, "Couldn't find key: %s", key);
		return 0;
	}
	if(err){
		ESP_LOGI(this->LOG_TAG, "ERR: %i", err);
		ESP_LOGE(this->LOG_TAG, "We couldn't fetch length from the NVS. key: %s. Error: %s", key, this->get_error(err));
		return -2;
	}
	if(len > maxLength){
		ESP_LOGE(this->LOG_TAG, "length too small. key: %s. len: %i maxLen: %i", key, len, maxLength);
		return -3;
	}
	err = nvs_get_str(this->nvsHandle, key, value, &len);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't fetch length from the NVS. key: %s. Error: %s", key, this->get_error(err));
		return -4;
	}
	return len;
}

int16_t PersistentMemory::getBlob(const char *key, void *value, size_t maxLength){
	if(this->initialized == 0 || !value || !key || maxLength == 0){
		ESP_LOGI(this->LOG_TAG, "init: %d, key: %s, maxLength: %u", this->initialized, key, maxLength);
		ESP_LOGW(this->LOG_TAG, "Trying to fetch from memory without key, opened or len.");
		return -1;
	}
	size_t len = 0;
	esp_err_t err = nvs_get_blob(this->nvsHandle, key, NULL, &len);
	if(err == ESP_ERR_NVS_NOT_FOUND){
		ESP_LOGW(this->LOG_TAG, "Couldn't find key: %s", key);
		return 0;
	}
	if(err){
		ESP_LOGI(this->LOG_TAG, "ERR: %i", err);
		ESP_LOGE(this->LOG_TAG, "We couldn't fetch length from the NVS. key: %s. Error: %s", key, this->get_error(err));
		return -2;
	}
	if(len > maxLength){
		ESP_LOGE(this->LOG_TAG, "length too small. key: %s. len: %i maxLen: %i", key, len, maxLength);
		return -3;
	}
	err = nvs_get_blob(this->nvsHandle, key, value, &len);
	if(err){
		ESP_LOGE(this->LOG_TAG, "We couldn't fetch length from the NVS. key: %s. Error: %s", key, this->get_error(err));
		return -4;
	}
	return len;
}

//int8_t PersistentMemory::keyExists()
