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
#include "images.h"
#include "cJSON.h"
#include "weather.h"
#include "backend.h"

Weather::~Weather(){
	this->reset();
}

uint8_t Weather::downloadWeatherForTown(const char *id){
	this->reset();

	Backend *b = new Backend();

	char *content = b->getWeather(id);
	cJSON *root = cJSON_Parse(content);
	free(content);
	delete b;

	cJSON *temperature = cJSON_GetObjectItemCaseSensitive(root, "temperature");
	cJSON *dewpoint = cJSON_GetObjectItemCaseSensitive(root, "dewpoint");
	cJSON *wind = cJSON_GetObjectItemCaseSensitive(root, "wind");
	cJSON *humidity = cJSON_GetObjectItemCaseSensitive(root, "humidity");
	cJSON *precipitation = cJSON_GetObjectItemCaseSensitive(root, "precipitation");

	cJSON *icon = cJSON_GetObjectItemCaseSensitive(root, "icon");
	cJSON *name = cJSON_GetObjectItemCaseSensitive(root, "name");

	if(cJSON_IsNumber(temperature)) this->temparature = temperature->valuedouble;
	if(cJSON_IsNumber(dewpoint)) this->dewpoint = dewpoint->valuedouble;
	if(cJSON_IsNumber(wind)) this->wind = wind->valuedouble * 3.6; // convert from m/s to km/h
	if(cJSON_IsNumber(humidity)) this->humidity = humidity->valuedouble;
	if(cJSON_IsNumber(precipitation)) this->precipitation = precipitation->valuedouble;

	if(cJSON_IsNumber(icon)) this->icon = icon->valueint;
	if(cJSON_IsString(name) && name->valuestring != NULL){
		uint8_t len = strlen(name->valuestring);
		this->name = (char *)malloc(len+1);
		strncpy(this->name, name->valuestring, len);
		this->name[len] = '\0';
	}

	cJSON_Delete(root);

	if(this->icon != 0)
		this->parseIcon();

	return 0;

}

void Weather::reset(){
	this->temparature = 0.0f;
	this->dewpoint = 0.0f;
	this->wind = 0.0f;
	this->humidity = 0.0f;
	this->precipitation = 0.0f;
	this->icon = 0;
	if(this->name != NULL)
		free(this->name);
	this->name = NULL;
}

void Weather::parseIcon(){
	// Sunny
	if(this->icon == 1 || this->icon == 2){
		this->drawableIcon = imageWeatherSun;
		this->drawableIconColor = rgb(255, 255, 0);
	}

	// Cloudy
	if(this->icon == 3 || this->icon == 4 || this->icon == 5){
		this->drawableIcon = imageWeatherCloud;
		this->drawableIconColor = rgb(150, 150, 150);
	}

	// Snowy
	if(this->icon == 7 || this->icon == 12 || this->icon == 15){
		this->drawableIcon = imageWeatherSnow;
		this->drawableIconColor = rgb(255, 255, 255);
	}

	// Foggy
	if(this->icon == 8 || this->icon == 14){
		this->drawableIcon = imageWeatherFog;
		this->drawableIconColor = rgb(200, 200, 200);
	}

	// light rainy
	if(this->icon == 10 || this->icon == 11){
		this->drawableIcon = imageWeatherLightRain;
		this->drawableIconColor = rgb(0, 150, 255);
	}

	// heavy rainy
	if(this->icon == 6 || this->icon == 13){
		this->drawableIcon = imageWeatherHeavyRain;
		this->drawableIconColor = rgb(0, 0, 255);
	}
}

double Weather::getTemparature(){
	return this->temparature;
}

double Weather::getDewPoint(){
	return this->dewpoint;
}

double Weather::getWind(){
	return this->wind;
}

double Weather::getHumidity(){
	return this->humidity;
}

double Weather::getPrecipitation(){
	return this->precipitation;
}

uint8_t Weather::getIcon(){
	return this->icon;
}

char *Weather::getName(){
	return this->name;
}

uint8_t *Weather::getDrawableIcon(){
	return this->drawableIcon;
}

rgbColor_t Weather::getDrawableIconColor(){
	return this->drawableIconColor;
}
