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

// #include <esp_log.h>
#include "button.h"
#include "auto_turn_off.h"

/**
 * Description of the constructor
 */
Button::Button(uint8_t pinL1, uint8_t pinL2, uint8_t pinL3,
			   uint8_t pinR1, uint8_t pinR2, uint8_t pinR3 )
{
	buttonPin[0] = pinL1;
	buttonPin[1] = pinL2;
	buttonPin[2] = pinL3;
	buttonPin[3] = pinR1;
	buttonPin[4] = pinR2;
	buttonPin[5] = pinR3;
}

/**
 * Description of the deconstructor
 */
Button::~Button()
{

}

/**
 * Returns a byte with the defined state of the buttons
 *
 * \param	l1	button L1
 * \param	l2	button L2
 * \param	l3	button L3
 * \param	r1	button R1
 * \param	r2	button R2
 * \param	r3	button R3
 *
 * \return byte with the defined button states
 */
uint8_t Button::createButtonByte(bool l1, bool l2, bool l3, bool r1, bool r2, bool r3)
{
	uint8_t buttonByte = 0;
	if (l1) buttonByte |= (1 << 0);
	if (l2) buttonByte |= (1 << 1);
	if (l3) buttonByte |= (1 << 2);
	if (r1) buttonByte |= (1 << 3);
	if (r2) buttonByte |= (1 << 4);
	if (r3) buttonByte |= (1 << 5);
	return buttonByte;
}

/**
 * returns a masked byte with the current state of the buttons
 *
 * \return byte witht he current button states
 */
uint8_t Button::getButtonByte()
{
	uint8_t buttonByte = 0;
	if (isPressed(L1)) buttonByte |= (1 << 0);
	if (isPressed(L2)) buttonByte |= (1 << 1);
	if (isPressed(L3)) buttonByte |= (1 << 2);
	if (isPressed(R1)) buttonByte |= (1 << 3);
	if (isPressed(R2)) buttonByte |= (1 << 4);
	if (isPressed(R3)) buttonByte |= (1 << 5);
	return buttonByte;
}

/**
 * returns state of the given button
 *
 * \return true if the button is pressed
 */
bool Button::isPressed(button_t button)
{
	bool pressed = false;
	if(button == L1)	// button L1 is inverted
		pressed = !digitalRead(buttonPin[button]);
	else
		pressed = digitalRead(buttonPin[button]);
	if (pressed)
	{
		AutoTurnOff& ato = AutoTurnOff::getInstance();
		ato.reset();
	}
	return pressed;
}
