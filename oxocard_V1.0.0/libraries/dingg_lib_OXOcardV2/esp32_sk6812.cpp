/*******************************************************************************
* \file    esp32_sk6812.cpp
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \date			05.03.2018
* \version 1.0
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
*******************************************************************************/

/* ================================= Imports ================================ */
#include <esp_log.h>
#include <stdio.h>
#include <xtensa/xtruntime.h>
#include <xtensa/core-macros.h>
#include "esp32_sk6812.h"
#include "sdkconfig.h"

/* ====================== Module class instantiations ======================= */
//...

/* ====================== Constructor & Deconstructor ======================= */
ESP32_SK6812::ESP32_SK6812(uint8_t ledPin, uint16_t ledCount)
{
	ledPin_ = gpio_num_t(ledPin);
	ledCount_ = ledCount;
	gpio_pad_select_gpio(ledPin_);
	gpio_set_direction(ledPin_, GPIO_MODE_OUTPUT);
}

ESP32_SK6812::~ESP32_SK6812()
{
	gpio_set_direction(ledPin_, GPIO_MODE_DISABLE);
}

/* ======================== Public member Functions ========================= */
void IRAM_ATTR ESP32_SK6812::setPixel(uint8_t r, uint8_t g, uint8_t b)
{
	sendByte(g);
	sendByte(r);
	sendByte(b);
}

void ESP32_SK6812::apply(FrameBuffer *frameBuffer)
{
	#if (F_CPU_IN_MHZ == 80)
		ESP_LOGE(this->LOG_TAG, "CPU_FREQ must be 160MHz or 240MHz to be able to control the LEDs!");
		while(1);
	#endif
	static const uint16_t TICS_PER_RESET = F_CPU_IN_MHZ * T_RESET_IN_US;
	uint32_t volatile register ilevel = XTOS_DISABLE_ALL_INTERRUPTS;
	for (uint8_t n = 0; n < ledCount_; n++)
	{
		rgbColor_t color = frameBuffer->getPixel(n);
		setPixel(color.r, color.g, color.b);
	}
	uint32_t startTime = XTHAL_GET_CCOUNT();
	uint32_t cpuTicks = startTime;
	while((cpuTicks - startTime) < TICS_PER_RESET) cpuTicks = XTHAL_GET_CCOUNT();	// wait for period end
	XTOS_RESTORE_INTLEVEL(ilevel);
}

/* ======================= Private member Functions ========================= */
void IRAM_ATTR ESP32_SK6812::sendByte(uint8_t colorByte)
{
	static const uint16_t TICS_PER_PERIOD = F_CPU_IN_MHZ * T_PERIOD_IN_US;
	static const uint8_t TICS_PER_T1H = F_CPU_IN_MHZ * T_1H_IN_US;
	static const uint8_t TICS_PER_T0H = F_CPU_IN_MHZ * T_0H_IN_US;
	uint32_t startTime = XTHAL_GET_CCOUNT();
	uint32_t cpuTicks = startTime;
	for (int8_t n = 7; n >= 0; n--)
	{
		while((cpuTicks - startTime) < TICS_PER_PERIOD) cpuTicks = XTHAL_GET_CCOUNT();	// wait for period end
		GPIO.out_w1ts = (1 << ledPin_);	// set GPIO
		startTime = cpuTicks;
		if (colorByte & (1 << n)) {
			while((cpuTicks - startTime) < TICS_PER_T1H) cpuTicks = XTHAL_GET_CCOUNT();	// wait for T1H
		}
		else {
			while((cpuTicks - startTime) < TICS_PER_T0H) cpuTicks = XTHAL_GET_CCOUNT();	// wait for T0H
		}
		GPIO.out_w1tc = (1 << ledPin_);	// clear GPIO
	}
	while((cpuTicks - startTime) < TICS_PER_PERIOD) cpuTicks = XTHAL_GET_CCOUNT();	// wait for period end
}
