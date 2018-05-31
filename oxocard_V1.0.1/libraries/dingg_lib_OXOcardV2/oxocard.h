#ifndef _LIB_OXOCARD_H_
#define _LIB_OXOCARD_H_ 1
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

#include <Arduino.h>
#include <esp_system.h>
#include <math.h>
#include "accelerometer.h"
#include "audio.h"
#include "battery.h"
#include "button.h"
#include "clock.h"
#include "communication.h"
#include "weather.h"
#include "matrix.h"
#include "auto_turn_off.h"
#include "system.h"
#include "list.h"
#include "random.h"
#include "version.h"
#include "wireless.h"

/* OTA configuration defines */
#define CONFIG_OTA_HOSTNAME			"api.oxocard.ch"
#define CONFIG_OTA_PATH_PREFIX	"/v1"
#define CONFIG_OTA_PORT					443

class OXOcard{
	public:
		System *system = NULL;
		Battery *battery = NULL;
		Button *button = NULL;
		Accelerometer *accelerometer = NULL;
		Audio *audio = NULL;
		Matrix *matrix = NULL;
		Wireless *wifi = NULL;
		Weather *weather = NULL;
		Communication::Local *communication = NULL;
		Clock *clock = NULL;
		List<int32_t> *numberList = NULL;
		List<rgbColor_t> *colorList = NULL;

		OXOcard();
		~OXOcard();

		void init();

	protected:

	private:

};

#endif
