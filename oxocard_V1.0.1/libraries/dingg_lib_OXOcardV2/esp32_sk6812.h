#ifndef _ESP32_SK6812_H_
#define _ESP32_SK6812_H_
/*******************************************************************************
* \file		esp32_sk6812.h
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \date			05.03.2018
* \version	1.0
*
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
********************************************************************************
* OXOcard2
*******************************************************************************/

/* ============================== Global imports ============================ */
#include <driver/gpio.h>
#include "frame_buffer.h"

/* ========================== Global module typedefs ======================== */
//...

/* ==================== Global module constant declaration ================== */
#define F_CPU_IN_MHZ	CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ	// see sdkconfig

/* ========================= Global macro declaration ======================= */
//...

/* ============================ Class declaration =========================== */
class ESP32_SK6812
{
public:
	/* Public member typedefs */
	//...

	/* Public constant declerations (static) */
	//...

	/* Constructor and Destructor */
	ESP32_SK6812(uint8_t ledPin, uint16_t ledCount);
	~ESP32_SK6812();

	/* Public member functions */
	void setPixel(uint8_t r, uint8_t g, uint8_t b);
	void apply(FrameBuffer* frameBuffer);

private:
	/* Private member typedefs */
	//...

	/* Private constant declerations (static) */
	const char *LOG_TAG = "LEDs";

	// SK6812 specific constants
	const float T_PERIOD_IN_US = 1.25;
	const float T_1H_IN_US = 0.6;
	const float T_0H_IN_US = 0.3;
	const float T_RESET_IN_US = 7;	// should be >= 50us but 7us works fine too...
	// ...see: https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/

	/* Private member data */
	gpio_num_t ledPin_;
	uint16_t ledCount_;

	/* Private member functions */
	void sendByte(uint8_t color);

	/* Private class functions (static) */
	//...

};

#endif
