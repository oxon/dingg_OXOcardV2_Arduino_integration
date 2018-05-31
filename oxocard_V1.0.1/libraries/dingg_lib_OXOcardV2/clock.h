#ifndef _LIB_CLOCK_H_
#define _LIB_CLOCK_H_ 1
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
#include <list>
#include <stdint.h>
#include "cJSON.h"

class TimeZone{
	public:
		TimeZone();
		TimeZone(const cJSON *json);
		~TimeZone();
		const char *getName();
		const char *getDescription();
		uint8_t getId();
		int32_t getOffset();

	protected:

	private:
		const char *LOG_TAG = "TimeZone";
		char *name = NULL;
		char *description = NULL;
		uint8_t id = 0;
		int32_t offset = 0;
};

class Clock{

	public:
		Clock();
		~Clock();

		int8_t init();
		void setTimeZone(uint8_t id);
		uint8_t getSecond();
		uint8_t getMinute();
		uint8_t getHour();
		uint8_t getWeekDay();
		uint8_t getDay();
		uint8_t getMonth();
		uint16_t getYear();

	protected:

	private:
		const char *LOG_TAG = "Clock";
		static const uint8_t DEFAULT_TIME_ZONE_ID = 12;	// switzerland

		TimeZone *currentTimezone = NULL;
		std::list<TimeZone *> timezones;
		bool initialized = false;

		struct tm getCurrentUTC(void);
		void debugTime(void);

};

#endif
