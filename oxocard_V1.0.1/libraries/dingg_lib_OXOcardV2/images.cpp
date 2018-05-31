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

#include "images.h"

uint8_t imageWlanIcon[8] = {
	0b00000000,
	0b01111110,
	0b10000001,
	0b00111100,
	0b01000010,
	0b00011000,
	0b00011000,
	0b00000000
};

uint8_t imageDownloadIcon[8] = {
	0b00000000,
	0b00000000,
	0b01000010,
	0b00100100,
	0b00011000,
	0b00000000,
	0b00111100,
	0b00000000
};

uint8_t imageRefreshIcon[8] = {
	0b00011100,
	0b10100010,
	0b11000001,
	0b11100000,
	0b00000111,
	0b10000011,
	0b01000101,
	0b00111000
};


uint8_t imageWeatherSun[8] = {
	0b10011001,
	0b01000010,
	0b00111100,
	0b10111101,
	0b10111101,
	0b00111100,
	0b01000010,
	0b10011001,
};

uint8_t imageWeatherCloud[8] = {
	0b00000000,
	0b00000000,
	0b01111110,
	0b11111111,
	0b11111111,
	0b01111110,
	0b00000000,
	0b00000000,
};

uint8_t imageWeatherSnow[8] = {
	0b10001010,
	0b10100010,
	0b00101000,
	0b10000010,
	0b00101000,
	0b10100010,
	0b10001000,
	0b00101010,
};

uint8_t imageWeatherFog[8] = {
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
};

uint8_t imageWeatherLightRain[8] = {
	0b10101010,
	0b00100000,
	0b10001010,
	0b00101000,
	0b10000010,
	0b10101000,
	0b00000010,
	0b10001000,
};

uint8_t imageWeatherHeavyRain[8] = {
	0b10101010,
	0b10101010,
	0b10000010,
	0b00101010,
	0b10100000,
	0b10101010,
	0b10101010,
	0b00001010,
};

uint8_t imageCheckmarkIcon[8] = {
	0b00000000,
	0b00000000,
	0b00000010,
	0b00000100,
	0b00101000,
	0b00010000,
	0b00000000,
	0b00000000
};

uint8_t imageXIcon[8] = {
	0b00000000,
	0b00000000,
	0b00100100,
	0b00011000,
	0b00011000,
	0b00100100,
	0b00000000,
	0b00000000
};

uint8_t imageConnectIcon[8] = {
	0b00000000,
	0b00000000,
	0b00100100,
	0b01000010,
	0b11111111,
	0b01000010,
	0b00100100,
	0b00000000
};

rgbColor_t oxocardBrandColors[5] = {
	rgb(122,29,144),	// purple
	rgb(102,197,48),	// green
	rgb(237,5,119),		// red
	rgb(243,92,25),		// orange
	rgb(25,181,241)		// blue
};

rgbColor_t scaleColor(rgbColor_t original, float scale){
	return rgb(original.r*scale, original.g*scale, original.b*scale);
}

void drawOxocardLogo(Matrix *matrix, float scale){
	matrix->clear();
	matrix->disableAutoUpdate();
	matrix->drawRectangle(1,1,2,2,scaleColor(oxocardBrandColors[0], scale));
	matrix->drawRectangle(5,1,2,2,scaleColor(oxocardBrandColors[1], scale));
	matrix->drawRectangle(3,3,2,2,scaleColor(oxocardBrandColors[2], scale));
	matrix->drawRectangle(1,5,2,2,scaleColor(oxocardBrandColors[3], scale));
	matrix->drawRectangle(5,5,2,2,scaleColor(oxocardBrandColors[4], scale));
	matrix->update();
	matrix->enableAutoUpdate();
	matrix->clear();
}
