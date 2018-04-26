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
#include "random.h"

uint8_t Random::getByte(){
	static uint32_t currentPool = 0;
	static uint8_t poolIndex = 255;
	if(poolIndex >=24){
		currentPool = getInt();
		poolIndex = 0;
	}
	uint8_t r = (uint8_t)(currentPool >> poolIndex);
	poolIndex += 8;
	return r;
}

uint32_t Random::getInt(uint32_t max){
	return (esp_random() % max);
}

uint32_t Random::getInt(uint32_t min, uint32_t max){
	return (min + (esp_random() % (max - min)));
}

rgbColor_t Random::getColor() {
	rgbColor_t color;
	switch(getInt(6+1))
	{
		case 0: color = rgb(255, 0, getInt(255+1)); break;
		case 1: color = rgb(255, getInt(255+1), 0); break;
		case 2: color = rgb(0, 255, getInt(255+1)); break;
		case 3: color = rgb(getInt(255+1), 255, 0); break;
		case 4: color = rgb(0, getInt(255+1), 255); break;
		case 5: color = rgb(getInt(255+1), 0, 255); break;
		case 6: color = rgb(getInt(85, 255+1), getInt(85, 255+1), getInt(85, 255+1)); break;
	}
	return color;
}
