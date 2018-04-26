#ifndef _LIB_AUTO_TURN_OFF_H_
#define _LIB_AUTO_TURN_OFF_H_ 1
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
#include "system.h"

/* AutoTurnOff defines */
constexpr uint16_t DEFAULT_AUTO_TURN_OFF_SECONDS	= 120;
constexpr uint8_t AUTO_TURN_OFF_TIMER_NR					= 0;

/* Singleton class */
class AutoTurnOff{

	public:
		static AutoTurnOff& getInstance(){
			static AutoTurnOff instance;
			return instance;
		}

		// this prevents creating an object by other means instead of
		// using: AutoTurnOff::getInstance()
		AutoTurnOff(AutoTurnOff const&)			= delete;	// C++ 11
		void operator=(AutoTurnOff const&)	= delete;	// C++ 11

		void enable();
		void disable();
		void configure(uint16_t seconds = DEFAULT_AUTO_TURN_OFF_SECONDS,
			uint8_t buttonByte = DEFAULT_TURN_OFF_BUTTON_BYTE);
		bool isTriggered();
		void reset();

	protected:

	private:
		// private de-/constructor
		AutoTurnOff();
		~AutoTurnOff();

		const char *LOG_TAG = "AutoTurnOff";
		System *system = NULL;
		uint16_t autoTurnOffSeconds = DEFAULT_AUTO_TURN_OFF_SECONDS;
		uint8_t autoTurnOffButtonByte = DEFAULT_TURN_OFF_BUTTON_BYTE;
		uint64_t atoMillisRef = 0;

		static void autoTurnOffHandler();

};

#endif
