#ifndef _LIB_PAIRING_SCENARIO_H_
#define _LIB_PAIRING_SCENARIO_H_ 1
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

#include <esp_system.h>
#include <stdint.h>
#include "images.h"
#include "rgb_leds.h"
#include "scenario.h"

/* FreeRTSO includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class PairingScenario : public Scenario{
	public:
		PairingScenario(OXOcard *card) : Scenario(card){};
		void wlanIconAnimation();
		void deleteAnimationsTask();
		TaskHandle_t animationsHandle = NULL;
		volatile bool deleteAnimationsTaskRequest = false;

	protected:
		int8_t configure(void);
		int8_t execute(void);

	private:
		const char *LOG_TAG = "PairingScenario";
		int8_t parseAndDrawChallenge(const char *challengeString);
		rgbColor_t challengeColorMap[6] = {
			oxocardBrandColors[4],  // blue
			oxocardBrandColors[2],  // red
			rgb(255, 221, 0),       // yellow
			rgb(255, 255, 255),     // white
			oxocardBrandColors[1],  // green
			rgb(0, 0, 0)            // black
		};

};

#endif
