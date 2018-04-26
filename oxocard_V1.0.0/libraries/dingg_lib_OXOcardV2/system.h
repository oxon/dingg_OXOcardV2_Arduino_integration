#ifndef _LIB_SYSTEM_H_
#define _LIB_SYSTEM_H_
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
#include <nvs_flash.h>
#include "accelerometer.h"
#include "audio.h"
#include "matrix.h"
#include "uuid.h"
#include "version.h"

/* Sleep defines */
constexpr uint8_t DEFAULT_TURN_OFF_BUTTON_BYTE = 0b00111111;

class System{
	public:

		const static uint8_t DEVICE_NAME_LENGTH = 32;

		enum wakeUpCause_t : uint8_t {
			WUC_UNDEF=0,
			WUC_BUTTON_L1,
			WUC_BUTTON_L2,
			WUC_BUTTON_L3,
			WUC_BUTTON_R1,
			WUC_BUTTON_R2,
			WUC_BUTTON_R3,
			WUC_INT_RTC,
			WUC_AP_MODE_REQUEST,
			WUC_UPDATE_REQUEST,
			WUC_PAIRING_REQUEST,
			WUC_DEBUG_MODE_REQUEST
		};

		const char wakeUpCauseText[12][20] = {
			"Undefined",
			"Button L1",
			"Button L2",
			"Button L3",
			"Button R1",
			"Button R2",
			"Button R3",
			"Internal RTC",
			"AP mode request",
			"OTA update request",
			"Pairing request",
			"Debug mode request"
		};

		System(Accelerometer *accel, Audio *audio, Matrix *matrix);
		~System();

		void init();

		void turnOff(uint8_t buttonByte = DEFAULT_TURN_OFF_BUTTON_BYTE);
		void turnOffFor(uint64_t us, uint8_t buttonByte = DEFAULT_TURN_OFF_BUTTON_BYTE);
		void reboot(void);
		wakeUpCause_t getWakeUpCause();
		const char* getWakeUpCauseText();

		static char *getId();
		static char *getName();
		static int8_t setName(char *name);
		static void printInfo();

	protected:

	private:
		const char *LOG_TAG = "System";
		Accelerometer *accel = NULL;
		Audio *audio = NULL;
		Matrix *matrix = NULL;
		static char deviceId[];
		static char deviceName[];

		void initPins();
		void disablePeriphery();
		void deepSleep(uint64_t us, uint8_t buttonByte);
		void showTurnOff();

};

#endif
