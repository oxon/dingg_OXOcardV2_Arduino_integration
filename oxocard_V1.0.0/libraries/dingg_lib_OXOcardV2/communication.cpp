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
#include <lwip/ip_addr.h>
#include <string.h>
#include "communication.h"

using namespace Communication;

static const char prefix[] = {29,31,111,120,111,99,97,114,100,29,31}; //\x1d\x1foxocard\x1d\x1f
const char *LocalMessage::PREFIX = prefix;

LocalMessage::LocalMessage(const char *message){
	this->initFromMessage(message);
}

LocalMessage::LocalMessage(const char *sender, const char *userMessage){
	this->initFromParams(sender, NULL, userMessage, 0);
}

LocalMessage::LocalMessage(const char *sender, const char *userMessage, const char *receiver){
	this->initFromParams(sender, receiver, userMessage, 0);
}

LocalMessage::LocalMessage(const char *sender, const char *userMessage, const char *receiver, uint32_t userNumber){
	this->initFromParams(sender, receiver, userMessage, userNumber);
}

LocalMessage::~LocalMessage(){
	if(this->sender != NULL)
		free(this->sender);
	if(this->receiver != NULL)
		free(this->receiver);
	if(this->userMessage != NULL)
		free(this->userMessage);
	if(this->message != NULL)
		free(this->message);
}

int8_t LocalMessage::initFromMessage(const char *message){
	if(message != NULL){
		size_t messageLength = strlen(message);
		this->message = (char *)malloc(messageLength+1);
		strncpy(this->message, message, messageLength);
		this->message[messageLength] = '\0';
	}else{
		this->message = (char *)malloc(sizeof(""));
		this->message[0] = '\0';
	}
	this->parseMessage();
	return 0;
}

int8_t LocalMessage::initFromParams(const char *sender, const char *receiver, const char *userMessage, int32_t userNumber){

	if(sender != NULL){
		size_t senderLength = strlen(sender);
		this->sender = (char *)malloc(senderLength+1);
		strncpy(this->sender, sender, senderLength);
		this->sender[senderLength] = '\0';
	}else{
		this->sender = (char *)malloc(sizeof(""));
		this->sender[0] = '\0';
	}

	if(receiver != NULL){
		size_t receiverLength = strlen(receiver);
		this->receiver = (char *)malloc(receiverLength+1);
		strncpy(this->receiver, receiver, receiverLength);
		this->receiver[receiverLength] = '\0';
	}else{
		this->receiver = (char *)malloc(sizeof(""));
		this->receiver[0] = '\0';
	}

	if(userMessage != NULL){
		size_t userMessageLength = strlen(userMessage);
		this->userMessage = (char *)malloc(userMessageLength+1);
		strncpy(this->userMessage, userMessage, userMessageLength);
		this->userMessage[userMessageLength] = '\0';
	}else{
		this->userMessage = (char *)malloc(sizeof(""));
		this->userMessage[0] = '\0';
	}

	this->userNumber = userNumber;

	this->craftMessage();
	return 0;
}

int8_t LocalMessage::craftMessage(){

	cJSON *root = cJSON_CreateObject();

	cJSON_AddItemToObject(root, "s", cJSON_CreateString(this->sender));
	cJSON_AddItemToObject(root, "r", cJSON_CreateString(this->receiver));
	cJSON_AddItemToObject(root, "um", cJSON_CreateString(this->userMessage));
	cJSON_AddItemToObject(root, "un", cJSON_CreateNumber(this->userNumber));

	char *src = cJSON_PrintUnformatted(root);
	size_t messageLength = strlen(src) + strlen(LocalMessage::PREFIX);

	this->message = (char *)malloc(messageLength+1);
	snprintf(this->message, messageLength+1, "%s%s", LocalMessage::PREFIX, src);
	free(src);
	cJSON_Delete(root);
	return 0;
}

int8_t LocalMessage::parseMessage(){

	if(strncmp(this->message, LocalMessage::PREFIX, strlen(LocalMessage::PREFIX)) != 0){
		ESP_LOGW(this->LOG_TAG, "We couldn't find prefix. So not parsing.");
		return -1;
	}

	cJSON *root = cJSON_Parse(this->message+strlen(LocalMessage::PREFIX));

	cJSON *sender = cJSON_GetObjectItemCaseSensitive(root, "s");
	if(cJSON_IsString(sender)){
		size_t len = strlen(sender->valuestring);
		this->sender = (char *)malloc(len+1);
		strncpy(this->sender, sender->valuestring, len);
		this->sender[len] = '\0';
	}

	cJSON *receiver = cJSON_GetObjectItemCaseSensitive(root, "r");
	if(cJSON_IsString(receiver)){
		size_t len = strlen(receiver->valuestring);
		this->receiver = (char *)malloc(len+1);
		strncpy(this->receiver, receiver->valuestring, len+1);
		this->receiver[len] = '\0';
	}

	cJSON *userMessage = cJSON_GetObjectItemCaseSensitive(root, "um");
	if(cJSON_IsString(userMessage)){
		size_t len = strlen(userMessage->valuestring);
		this->userMessage = (char *)malloc(len+1);
		strncpy(this->userMessage, userMessage->valuestring, len+1);
		this->userMessage[len] = '\0';
	}

	cJSON *userNumber = cJSON_GetObjectItemCaseSensitive(root, "un");
	if(cJSON_IsNumber(userNumber)) this->userNumber = userNumber->valueint;

	cJSON_Delete(root);
	return 0;
}

Local::Local(){
	this->init();
}

Local::~Local(){
	if(this->filterSenderName != NULL)
		free(this->filterSenderName);
	if(this->udp != NULL) delete this->udp;
	if(this->message != NULL) delete this->message;
}

void Local::init(){
	this->udp = new WiFiUDP();
	this->hasBegun = 0;
}

void Local::start(const char *myName){
	this->myName = myName;
	this->reconfigure();

	if(this->hasBegun == 0){
		this->udp->begin(Local::UDP_PORT);
		this->hasBegun = 1;
	}
}

void Local::stop(){
	if(this->hasBegun == 1){
		this->udp->stop();
		this->hasBegun = 0;
	}
}

int8_t Local::reconfigure(){
	IPAddress mask = WiFi.subnetMask();
	this->broadcast = IPAddress(WiFi.localIP());
	for(uint8_t i=0; i<4; i++)
		this->broadcast[i] |= ~mask[i];
	ESP_LOGI(this->LOG_TAG, "We have Broadcast: %d.%d.%d.%d",
		this->broadcast[0],
		this->broadcast[1],
		this->broadcast[2],
		this->broadcast[3]);
	return 0;
}

int8_t Local::readPacket(){
	if(this->udp == NULL || this->hasBegun == 0) return -1;

	uint16_t length = 0;

	while((length = this->udp->parsePacket()) > 0){
		char rawMessage[length+1];
		if(this->udp->read(rawMessage, length) != length){
			ESP_LOGW(this->LOG_TAG, "We have wrong lenghts. Dropping message.");
			this->udp->flush();
			return -3;
		}else{
			rawMessage[length] = '\0';
			if(this->message != NULL) delete this->message;
			this->message = new LocalMessage(rawMessage);
			this->udp->flush();
			return 1;
		}
	}
	return 0;
}

int8_t Local::writePacket(const char *receiver, const char *message, int32_t number){
	if(this->udp == NULL || this->hasBegun == 0) return -1;
	LocalMessage lm = LocalMessage(this->myName, message, receiver, number);
	this->udp->beginPacket(this->broadcast,Local::UDP_PORT);
	this->udp->write((uint8_t *)lm.message, strlen(lm.message));
	this->udp->endPacket();
	return 0;
}

int8_t Local::waitForMessages(uint16_t timeout){
	uint8_t waitedFor = 0;
	uint8_t index = 0;
	uint8_t returnValue = 0;
	while(timeout >= waitedFor || timeout == 65535){
		returnValue = this->readPacket();
		if(returnValue != 0)
			return returnValue;
		if(index == 20){
			waitedFor++;
			index = 0;
		}
		index ++;
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
	return 0;
}

int8_t Local::waitForMessages(){
	return this->waitForMessages(65535);
}


// DIRECT
int8_t Local::sendDirectMessageAndNumber(const char *message, int32_t number, const char *receiver){
	return this->writePacket(receiver, message, number);
}

int8_t Local::sendDirectMessage(const char *message, const char *receiver){
	return this->writePacket(receiver, message, 0);
}

int8_t Local::sendDirectNumber(int32_t number, const char *receiver){
	return this->writePacket(receiver, NULL, number);
}

int8_t Local::setDirectMessageFilter(const char *filterSender){
	if(filterSender != NULL){
		if(this->filterSenderName != NULL) free(this->filterSenderName);
		size_t len = strlen(filterSender);
		this->filterSenderName = (char *)malloc(len+1);
		strncpy(this->filterSenderName, filterSender, len);
		this->filterSenderName[len] = '\0';
	}else{
		this->filterSenderName = NULL;
		return -1;
	}
	return 0;
}

int8_t Local::isDirectMessageAndNotFiltered(){
	if(this->message == NULL) return -1;
	if(this->filterSenderName != NULL){
		size_t length = strlen(this->filterSenderName);
		if(strncmp(this->message->sender, this->filterSenderName, length) != 0)
			return 0;
		if(length != strlen(this->message->sender))
			return 0;
	}

	size_t length = strlen(this->myName);
	if(strncmp(this->message->receiver, this->myName, length) != 0)
		return 0;
	if(length != strlen(this->message->receiver))
		return 0;
	return 1;
}

int8_t Local::compareDirectMessageFrom(const char *message, const char *filterSender){
	setDirectMessageFilter(filterSender);
	return compareDirectMessage(message);
}

int8_t Local::compareDirectMessage(const char *message){
	if(this->message == NULL) return 0;
	if(this->isDirectMessageAndNotFiltered() == 0) return 0;
	if(this->message->userMessage == NULL) return 0;
	size_t length = strlen(this->message->userMessage);
	if(strncmp(this->message->userMessage, message, length) != 0)
		return 0;
	if(length != strlen(message))
		return 0;
	return 1;
}

int8_t Local::compareDirectNumber(int32_t number){
	if(this->message == NULL) return 0;
	if(this->isDirectMessageAndNotFiltered() == 0) return 0;
	if(this->message->userNumber != number) return 0;
	return 1;
}

char *Local::getDirectMessage(){
	if(this->message == NULL) return NULL;
	if(this->isDirectMessageAndNotFiltered() == 0) return NULL;
	return this->message->userMessage;
}

int32_t Local::getDirectNumber(){
	if(this->message == NULL) return 0;
	if(this->isDirectMessageAndNotFiltered() == 0) return 0;
	return this->message->userNumber;
}

// BROADCAST
int8_t Local::sendBroadcastMessageAndNumber(const char *message, int32_t number){
	return this->writePacket(NULL, message, number);
}

int8_t Local::sendBroadcastMessage(const char *message){
	return this->writePacket(NULL, message, 0);
}

int8_t Local::sendBroadcastNumber(int32_t number){
	return this->writePacket(NULL, NULL, number);
}

int8_t Local::isBroadcastMessage(){
	if(this->message == NULL) return 0;
	if(this->message->receiver != NULL) return 0;
	return 1;
}

int8_t Local::compareBroadcastMessage(const char *message){
	if(this->message == NULL) return 0;
	if(this->isBroadcastMessage() == 0) return 0;
	if(this->message->userMessage == NULL) return 0;
	size_t length = strlen(this->message->userMessage);
	if(strncmp(this->message->userMessage, message, length) != 0)
		return 0;
	if(length != strlen(message))
		return 0;
	return 1;
}

int8_t Local::compareBroadcastNumber(int32_t number){
	if(this->message == NULL) return 0;
	if(this->isBroadcastMessage() == 0) return 0;
	if(this->message->userNumber != number) return 0;
	return 1;
}

char *Local::getBroadcastMessage(){
	if(this->message == NULL) return 0;
	if(this->isBroadcastMessage() == 0) return NULL;
	return this->message->userMessage;
}

int32_t Local::getBroadcastNumber(){
	if(this->message == NULL) return 0;
	if(this->isBroadcastMessage() == 0) return 0;
	return this->message->userNumber;
}
