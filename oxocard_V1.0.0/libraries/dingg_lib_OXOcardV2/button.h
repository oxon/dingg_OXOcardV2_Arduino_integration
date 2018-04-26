#ifndef _LIB_BUTTON_H_
#define _LIB_BUTTON_H_
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

typedef enum : uint8_t
{
	L1 = 0,
	L2,
	L3,
	R1,
	R2,
	R3
} button_t;

/**
 * Description of the class
 */
class Button
{
public:

	Button(uint8_t pinL1, uint8_t pinL2, uint8_t pinL3,
		   uint8_t pinR1, uint8_t pinR2, uint8_t pinR3 );
	~Button();

	static uint8_t createButtonByte(bool l1, bool l2, bool l3, bool r1, bool r2, bool r3);

	uint8_t getButtonByte();
	bool isPressed(button_t button);

protected:


private:
	uint8_t buttonPin[6];

};

#endif
