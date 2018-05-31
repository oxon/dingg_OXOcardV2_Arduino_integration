#ifndef _LIB_ACCELEROMETER_H_
#define _LIB_ACCELEROMETER_H_
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

#include "LIS3DE.h"

class Accelerometer
{
	public:
		Accelerometer(uint8_t int1Pin = -1, uint8_t int2Pin = -1);
		~Accelerometer();

		void init();
		void disable();
		int8_t getOrientation();
		bool isOrientation(LIS3DE::orientation_t orientation);

		int8_t getX();
		int8_t getY();
		int8_t getZ();

		void setRootCursor(uint8_t xPos, uint8_t yPos);
		uint8_t getCursorX();
		uint8_t getCursorY();

		int8_t getTemperature();

	protected:

	private:
		const char *LOG_TAG = "Accelerometer";
		static const uint8_t CURSOR_THRESHOLD = 20;
		static const uint8_t ATO_ACCEL_THRESHOLD = 5;

		LIS3DE *driver = NULL;
		LIS3DE::orientation_t lastOrientation = LIS3DE::UNKNOWN;

		int8_t xCursor = 0;
		int8_t yCursor = 7;
		int8_t prevX = 0;
		int8_t prevY = 0;
		int8_t prevZ = 0;

};

#endif
