#ifndef _LIB_WEATHER_H_
#define _LIB_WEATHER_H_
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
#include "matrix.h"

class Weather{

	public:
		Weather(){};
		~Weather();

		uint8_t downloadWeatherForTown(const char *id);

		double getTemparature();
		double getDewPoint();
		double getWind();
		double getHumidity();
		double getPrecipitation();

		uint8_t getIcon();
		char *getName();

		uint8_t *getDrawableIcon();
		rgbColor_t getDrawableIconColor();

	protected:

	private:
		const char *LOG_TAG = "Weather";
		void parseIcon();
		void reset();

		char *name = NULL;
		double temparature;
		double dewpoint;
		double wind;
		double humidity;
		double precipitation;
		uint8_t icon;
		uint8_t *drawableIcon;
		rgbColor_t drawableIconColor;
};

#endif
