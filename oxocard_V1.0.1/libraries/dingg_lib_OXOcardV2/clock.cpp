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
#include "clock.h"
#include "backend.h"

/* time includes */
#include <sys/time.h>
#include <time.h>

/*
https://en.wikipedia.org/wiki/List_of_UTC_time_offsets

{
	"UTC-12:00": "UTC-12:00",
	"UTC-11:00": "Pago Pago",
	"UTC-10:00": "Honolulu, Papeete",
	"UTC-09:00": "Anchorage",
	"UTC-08:00": "Los Angeles, Tijuana, Vancouver"
	"UTC-07:00": "Calgary, Ciudad Juárez, Phoenix"
	"UTC-06:00": "Chicago, Guatemala City, Mexico City, San José, San Salvador, Winnipeg"
	"UTC-05:00": "New York, Lima, Toronto, Bogotá, Havana, Kingston"
	"UTC-04:00": "Caracas, Santiago, La Paz, Manaus, Halifax, Santo Domingo"
	"UTC-03:00": "Buenos Aires, Montevideo, São Paulo"
	"UTC-02:00": "UTC-02:00"
	"UTC-01:00": "Praia"
	"UTC+00:00": "Accra, Casablanca, Dakar, Dublin, Lisbon, London"
	"UTC+01:00": "Berlin, Zürich, Madrid, Paris, Rome, Tunis, Vienna, Warsaw"
	"UTC+02:00": "Athens, Bucharest, Cairo, Helsinki, Jerusalem, Johannesburg, Kiev"
	"UTC+03:00": "Istanbul, Moscow, Nairobi, Baghdad, Doha, Minsk, Riyadh"
	"UTC+04:00": "Baku, Dubai, Samara, Muscat"
	"UTC+05:00": "Karachi, Tashkent, Yekaterinburg"
	"UTC+06:00": "Almaty, Dhaka, Omsk"
	"UTC+07:00": "Jakarta, Bangkok, Krasnoyarsk, Ho Chi Minh City"
	"UTC+08:00": "Beijing, Hong Kong, Taipei, Singapore, Kuala Lumpur, Perth, Manila, Denpasar, Irkutsk"
	"UTC+09:00": "Seoul, Tokyo, Ambon, Yakutsk"
	"UTC+10:00": "Port Moresby, Brisbane, Vladivostok"
	"UTC+11:00": "Sydney, Nouméa"
	"UTC+12:00": "Auckland, Suva"
}

[
	{
		"id":1,
		"timezone":"Pacific/Samoa",
		"description": "Pago Pago"
	},
	{
		"id":2,
		"timezone":"Pacific/Honolulu",
		"description": "Honolulu, Papeete"
	},
	{
		"id":3,
		"timezone":"Pacific/Auckland",
		"description": "Anchorage"
	},
	{
		"id":4,
		"timezone":"America/Los_Angeles",
		"description": "Los Angeles, Vancouver"
	},
	{
		"id":5,
		"timezone":"America/Phoenix",
		"description": "Calgary, Phoenix"
	},
	{
		"id":6,
		"timezone":"America/Chicago",
		"description": "Chicago, Mexico City"
	},
	{
		"id":7,
		"timezone":"America/New_York",
		"description": "New York, Toronto"
	},
	{
		"id":8,
		"timezone":"America/Santiago",
		"description": "Santiago, Halifax"
	},
	{
		"id":9,
		"timezone":"America/Sao_Paulo",
		"description": "Buenos Aires, São Paulo"
	},
	{
		"id":10,
		"timezone":"Atlantic/Cape_Verde",
		"description": "Praia"
	},
	{
		"id":11,
		"timezone":"Europe/London",
		"description": "Lisbon, London"
	},
	{
		"id":12,
		"timezone":"Europe/Zurich",
		"description": "Zürich, Warsaw"
	},
	{
		"id":13,
		"timezone":"Europe/Athens",
		"description": "Athens, Helsinki"
	},
	{
		"id":14,
		"timezone":"Europe/Moscow",
		"description": "Moscow, Baghdad"
	},
	{
		"id":15,
		"timezone":"Asia/Baku",
		"description": "Baku, Dubai"
	},
	{
		"id":16,
		"timezone":"Asia/Karachi",
		"description": "Karachi, Yekaterinburg"
	},
	{
		"id":17,
		"timezone":"Asia/Dhaka",
		"description": "Dhaka, Omsk"
	},
	{
		"id":18,
		"timezone":"Asia/Jakarta",
		"description": "Jakarta, Bangkok"
	},
	{
		"id":19,
		"timezone":"Asia/Kuala_Lumpur",
		"description": "Beijing, Kuala Lumpur"
	},
	{
		"id":20,
		"timezone":"Asia/Tokyo",
		"description": "Tokyo, Yakutsk"
	},
	{
		"id":21,
		"timezone":"Australia/Brisbane",
		"description": "Port Moresby, Brisbane"
	},
	{
		"id":22,
		"timezone":"Australia/Sydney",
		"description": "Sydney, Nouméa"
	},
	{
		"id":23,
		"timezone":"Pacific/Auckland",
		"description": "Auckland, Suva"
	}
]
*/


TimeZone::TimeZone(){

}

TimeZone::TimeZone(const cJSON *json){

	cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "timezone");
	if(cJSON_IsString(name)){
		size_t len = strlen(name->valuestring);
		this->name = (char *)malloc(len+1);
		strncpy(this->name, name->valuestring, len);
		this->name[len] = '\0';
	}

	cJSON *description = cJSON_GetObjectItemCaseSensitive(json, "description");
	if(cJSON_IsString(description)){
		size_t len = strlen(description->valuestring);
		this->description = (char *)malloc(len+1);
		strncpy(this->description, description->valuestring, len);
		this->description[len] = '\0';
	}

	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if(cJSON_IsNumber(id)) this->id = id->valueint;

	cJSON *offset = cJSON_GetObjectItemCaseSensitive(json, "offset");
	if(cJSON_IsNumber(offset)) this->offset = offset->valueint;
}

TimeZone::~TimeZone(){
	if(this->name != NULL){
		free(this->name);
		this->name = NULL;
	}
	if(this->description != NULL){
		free(this->description);
		this->description = NULL;
	}
}

const char *TimeZone::getName(){
	return this->name;
}

const char *TimeZone::getDescription(){
	return this->description;
}

uint8_t TimeZone::getId(){
	return this->id;
}

int32_t TimeZone::getOffset(){
	return this->offset;
}


Clock::Clock(){
}

Clock::~Clock(){
}

void Clock::setTimeZone(uint8_t id){
	this->currentTimezone = NULL;
	for(auto zone = this->timezones.begin(); zone != this->timezones.end(); ++zone) {
		if((*zone)->getId() == id)
			this->currentTimezone = *zone;
	}
}

uint8_t Clock::getSecond(){
	if(!this->initialized) this->init();
	return this->getCurrentUTC().tm_sec;
}

uint8_t Clock::getMinute(){
	if(!this->initialized) this->init();
	return this->getCurrentUTC().tm_min;
}

uint8_t Clock::getHour(){
	if(!this->initialized) this->init();
	return this->getCurrentUTC().tm_hour;
}

uint8_t Clock::getWeekDay(){
	if(!this->initialized) this->init();
	return this->getCurrentUTC().tm_wday;
}

uint8_t Clock::getDay(){
	if(!this->initialized) this->init();
	return this->getCurrentUTC().tm_mday;
}

uint8_t Clock::getMonth(){
	if(!this->initialized) this->init();
	return this->getCurrentUTC().tm_mon;
}

uint16_t Clock::getYear(){
	if (!initialized) this->init();
	return this->getCurrentUTC().tm_year + 1900;
}

struct tm Clock::getCurrentUTC(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t mtime = tv.tv_sec + this->currentTimezone->getOffset();
	struct tm mtm;
	localtime_r(&mtime, &mtm);
	return mtm;
}

void Clock::debugTime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t mtime = tv.tv_sec;
	struct tm mtm;
	localtime_r(&mtime, &mtm);
	const char* time_str = asctime(&mtm);
	ESP_LOGI(this->LOG_TAG, "Got time: %s", time_str);
	for(auto zone = this->timezones.begin(); zone != this->timezones.end(); ++zone) {
		TimeZone *z = *zone;
		ESP_LOGI(this->LOG_TAG, "We have a timezone: >%s< (%d) >%s< (%d)", z->getDescription(), strlen(z->getDescription()), z->getName(), strlen(z->getName()));
		ESP_LOGI(this->LOG_TAG, "offset and id: %d // %d", z->getOffset(), z->getId());
	}
}

int8_t Clock::init(){

	struct timeval nowUTC = { .tv_sec = 0, .tv_usec=0};
	Backend b = Backend();
	char *content = b.getTime();
	cJSON *root = cJSON_Parse(content);
	free(content);
	cJSON *utc = cJSON_GetObjectItemCaseSensitive(root, "utc");
	if(cJSON_IsNumber(utc))
	nowUTC.tv_sec = utc->valuedouble;
	settimeofday(&nowUTC, NULL);
	const cJSON *timezone = NULL;
	const cJSON *timezones = NULL;
	timezones = cJSON_GetObjectItemCaseSensitive(root, "timezones");
	cJSON_ArrayForEach(timezone, timezones){
		this->timezones.push_back(new TimeZone(timezone));
	}
	this->setTimeZone(DEFAULT_TIME_ZONE_ID);
	this->debugTime();
	initialized = true;
	return 0;
}
