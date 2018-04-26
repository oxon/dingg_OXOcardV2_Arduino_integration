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
#include "auto_turn_off.h"

hw_timer_t * autoTurnOffTimer = NULL;
volatile SemaphoreHandle_t autoTurnOffTimerSemaphore;
portMUX_TYPE autoTurnOffTimerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint16_t autoTurnOffCounter = 0;

AutoTurnOff::AutoTurnOff(){ }

AutoTurnOff::~AutoTurnOff(){ }

/**
 * Enables the autoTurnOff function
 */
void AutoTurnOff::enable()
{
	autoTurnOffTimerSemaphore = xSemaphoreCreateBinary();
	autoTurnOffTimer = timerBegin(AUTO_TURN_OFF_TIMER_NR, 40000, true);	// 80MHz; TimerNr = 0; divider = 40000; counting upwards
	timerAttachInterrupt(autoTurnOffTimer, AutoTurnOff::autoTurnOffHandler, true);	// true = repeat interrupt
	timerAlarmWrite(autoTurnOffTimer, 2000, true);	// alarm (interrupt) after 2000 counts
	timerAlarmEnable(autoTurnOffTimer);
	reset();
	atoMillisRef = millis();
	ESP_LOGI(this->LOG_TAG, "AutoTurnOff enabled. Trigger in %d seconds.",
	  autoTurnOffSeconds);
}

/**
 * Disables the autoTurnOff function
 */
void AutoTurnOff::disable()
{
	timerAlarmDisable(autoTurnOffTimer);
	ESP_LOGI(this->LOG_TAG, "AutoTurnOff disabled.");
}

/**
 * Configures the autoTurnOff variables
 *
 * \param  seconds	time in s
 * \param  buttonByte	masked byte to determine the wake-up buttons
 */
void AutoTurnOff::configure(uint16_t seconds, uint8_t buttonByte)
{
	reset();
	autoTurnOffSeconds = seconds;
	autoTurnOffButtonByte = buttonByte;
	ESP_LOGI(this->LOG_TAG, "AutoTurnOff configured. New trigger in %d seconds.",
	  autoTurnOffSeconds);
}

/**
 * Handles the functinality to turnOff after a defined time in s
 */
bool AutoTurnOff::isTriggered()
{
	bool turnOffTriggered = false;
	if (xSemaphoreTake(autoTurnOffTimerSemaphore, 0) == pdTRUE)
	{
		uint16_t currentAutoTurnOffSeconds = 0;
		/* read the interrupt counter */
		portENTER_CRITICAL(&autoTurnOffTimerMux);
		currentAutoTurnOffSeconds = autoTurnOffCounter;
		portEXIT_CRITICAL(&autoTurnOffTimerMux);
		if (currentAutoTurnOffSeconds >= autoTurnOffSeconds)
		{
			ESP_LOGI(this->LOG_TAG, "triggered after %d seconds.",
			  (uint32_t)((millis() - atoMillisRef)/1000));
			/* defined time expired without an action -> turnOff */
			turnOffTriggered = true;
		}
	}
	return turnOffTriggered;
}

/**
 * Resets the auto turn off counter
 */
void AutoTurnOff::reset()
{
	timerWrite(autoTurnOffTimer, 0);	// reset timer (timerRestart() -> didn't work)
	portENTER_CRITICAL(&autoTurnOffTimerMux);
	autoTurnOffCounter = 0;
	portEXIT_CRITICAL(&autoTurnOffTimerMux);
	ESP_LOGI(this->LOG_TAG, "reseted autoTurnOffCounter.");
}

/**
 * Handler for the autoTurnOff timer interrupt
 */
void IRAM_ATTR AutoTurnOff::autoTurnOffHandler()
{
	portENTER_CRITICAL_ISR(&autoTurnOffTimerMux);
	autoTurnOffCounter++;
	portEXIT_CRITICAL_ISR(&autoTurnOffTimerMux);
	xSemaphoreGiveFromISR(autoTurnOffTimerSemaphore, NULL);
}
