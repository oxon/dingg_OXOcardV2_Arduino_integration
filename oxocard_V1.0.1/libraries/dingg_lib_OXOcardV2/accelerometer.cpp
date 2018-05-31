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
#include <Wire.h>
#include "accelerometer.h"
#include "auto_turn_off.h"

Accelerometer::Accelerometer(uint8_t int1Pin, uint8_t int2Pin){
	this->driver = new LIS3DE(Wire, int1Pin, int2Pin);
}

Accelerometer::~Accelerometer(){
	delete(this->driver);
}

void Accelerometer::init(){
	ESP_LOGI(LOG_TAG, "Initializing");
	if(!this->driver->begin()){
		ESP_LOGE(LOG_TAG, "Initializing the accelerometer driver failed");
		return;
	}
	this->driver->enableMovementDetectionINT1(true, false); // must be initialized! (otherwise INT will be pulled LOW => +330uA)
}

void Accelerometer::disable(){
	this->driver->end();
}

int8_t Accelerometer::getOrientation(){
	LIS3DE::orientation_t currentOrientation = this->driver->getOrientation();
	if (currentOrientation != lastOrientation) {
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	lastOrientation = currentOrientation;
	return (int8_t)currentOrientation;
}

bool Accelerometer::isOrientation(LIS3DE::orientation_t orientation){
	LIS3DE::orientation_t currentOrientation = this->driver->getOrientation();
	if (currentOrientation != lastOrientation) {
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	lastOrientation = currentOrientation;
	return (currentOrientation == orientation);
}

void Accelerometer::setRootCursor(uint8_t xPos, uint8_t yPos){
	this->xCursor = xPos;
	this->yCursor = yPos;
}

int8_t Accelerometer::getX(){
	int8_t x = this->driver->getVectorX();
	if ((prevX-x) > ATO_ACCEL_THRESHOLD || (prevX-x) < -ATO_ACCEL_THRESHOLD) {
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	prevX = x;
	return x;
}

int8_t Accelerometer::getY(){
	int8_t y = this->driver->getVectorY();
	if ((prevY-y) > ATO_ACCEL_THRESHOLD || (prevY-y) < -ATO_ACCEL_THRESHOLD) {
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	prevY = y;
	return y;
}
int8_t Accelerometer::getZ(){
	int8_t z = this->driver->getVectorZ();
	if ((prevZ-z) > ATO_ACCEL_THRESHOLD || (prevZ-z) < -ATO_ACCEL_THRESHOLD) {
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	prevZ = z;
	return z;
}

uint8_t Accelerometer::getCursorX(){
	int16_t v[3];
	int16_t ax;
	this->driver->getAccelerationVector(v);
	ax = v[0];
	if (ax > CURSOR_THRESHOLD) {
		this->xCursor++;
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	if (ax < -CURSOR_THRESHOLD) {
		this->xCursor--;
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	if (this->xCursor < 0) this->xCursor = 0;
	if (this->xCursor > 7) this->xCursor = 7;
	return this->xCursor;
}

uint8_t Accelerometer::getCursorY(){
	int16_t v[3];
	int16_t ay;
	this->driver->getAccelerationVector(v);
	ay = v[1];
	if (ay > CURSOR_THRESHOLD) {
		this->yCursor--;
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	if (ay < -CURSOR_THRESHOLD) {
		this->yCursor++;
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	if (this->yCursor < 0) this->yCursor = 0;
	if (this->yCursor > 7) this->yCursor = 7;
	return this->yCursor;
}

int8_t Accelerometer::getTemperature(){
	return this->driver->getTemperature();
}
