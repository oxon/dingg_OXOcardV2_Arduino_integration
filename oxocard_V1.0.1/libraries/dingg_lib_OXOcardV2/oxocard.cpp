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

#include "oxocard.h"
#include "pin_defines.h"

OXOcard::OXOcard(){

}

OXOcard::~OXOcard(){
	delete(this->system);
	delete(this->battery);
	delete(this->button);
	delete(this->accelerometer);
	delete(this->audio);
	delete(this->matrix);
	delete(this->wifi);
	delete(this->weather);
	delete(this->communication);
	delete(this->clock);
	delete(this->numberList);
	delete(this->colorList);
}

void OXOcard::init(){
	this->button = new Button(
		BUTTON_L1,
		BUTTON_L2,
		BUTTON_L3,
		BUTTON_R1,
		BUTTON_R2,
		BUTTON_R3
	);
	this->accelerometer = new Accelerometer(ACCEL_INT);
	this->audio = new Audio(A_SHUTDOWN);
	this->matrix = new Matrix(LEDS_PIN, EN_LEDS_PIN);
	this->system = new System(this->accelerometer, this->audio, this->matrix);
	this->battery = new Battery(SCAN_BAT, CHARGED, RXD_PIN, TXD_PIN);
	this->wifi = new Wireless();
	this->weather = new Weather();
	this->communication = new Communication::Local();
	this->clock = new Clock();
	this->numberList = new List<int32_t>();
	this->colorList = new List<rgbColor_t>();
}
