#ifndef _LIB_MELODY_H_
#define _LIB_MELODY_H_
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
#include <list>
#include "tones.h"


/**
 * Description of the class
 */
class Melody
{
public:

	Melody(uint8_t bpm);
	~Melody();

	void addNote(Note note, Octave octave, float duration);
	tone_t getNextNote();
	void fromStart();
	void deleteMelody();

	void setBPM(uint8_t bpm) {this->bpm = bpm;}
	uint8_t getBPM() {return bpm;}
	bool hasNext() {return std::next(listIterator) != noteList.end();}

protected:


private:

	std::list<tone_t> noteList;
	std::list<tone_t>::iterator listIterator;

	uint8_t bpm;

};
#endif
