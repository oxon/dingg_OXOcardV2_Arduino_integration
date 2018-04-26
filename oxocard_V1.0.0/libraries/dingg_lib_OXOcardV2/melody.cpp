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

#include "melody.h"


Melody::Melody(uint8_t bpm)
{
	this->bpm = bpm;
	listIterator = noteList.begin();
}


Melody::~Melody()
{

}


void Melody::addNote(Note note, Octave octave, float duration)
{
	tone_t t;
	t.note = note;
	t.octave = octave;
	t.duration = duration;

	noteList.push_back(t);
}


tone_t Melody::getNextNote()
{
	if(listIterator != noteList.end())
		return *listIterator++;
	else
		return *listIterator;
}


void Melody::fromStart()
{
	listIterator = noteList.begin();
}


void Melody::deleteMelody()
{
	noteList.clear();
}
