/*******************************************************************************
* \file    noise_renderer.cpp
********************************************************************************
* \author  Jascha Haldemann 		jh@oxon.ch
* \date    29.11.2017
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
#include "noise_renderer.h"

/* ======================= Module constant declaration ====================== */
//...

/* ======================== Module macro declaration ======================== */
//...

/* ======================== Module global variables ========================= */
//...

/* ====================== Module class instantiations ======================= */
//...

/* ======================== Public member Functions ========================= */
/** -------------------------------------------------------------------------
* \brief  writes the next few calculated values into the given buffer
--------------------------------------------------------------------------- */
void NoiseRenderer::getNextValues(uint32_t buffer[])
{
	for (uint16_t n = 0; n < bufferSize_; n++)
	{
		uint8_t data = random((uint16_t)(amplitude_)+1);
		uint8_t invertedData = amplitude_ - data;
		buffer[n] = toI2sDac(data, invertedData);
	}
}

/* ======================= Private member Functions ========================= */

/* ======================== Private class Functions ========================= */
