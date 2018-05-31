#ifndef _LIB_IMAGES_H_
#define _LIB_IMAGES_H_
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

#include <stdint.h>
#include "matrix.h"

extern uint8_t imageWlanIcon[];
extern uint8_t imageDownloadIcon[];
extern uint8_t imageRefreshIcon[];
extern uint8_t imageWeatherSun[];
extern uint8_t imageWeatherCloud[];
extern uint8_t imageWeatherSnow[];
extern uint8_t imageWeatherFog[];
extern uint8_t imageWeatherLightRain[];
extern uint8_t imageWeatherHeavyRain[];
extern uint8_t imageCheckmarkIcon[];
extern uint8_t imageXIcon[];
extern uint8_t imageConnectIcon[];

extern void drawOxocardLogo(Matrix *matrix, float scale = 1);

extern rgbColor_t oxocardBrandColors[];

#endif
