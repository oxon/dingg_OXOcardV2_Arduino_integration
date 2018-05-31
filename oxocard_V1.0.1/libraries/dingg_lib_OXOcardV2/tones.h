#ifndef _LIB_TONES_H_
#define _LIB_TONES_H_
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

enum class Note
{
	REST	= 0,
	C			= 131,
	CS		= 139,
	D			= 147,
	DS		= 156,
	E			= 165,
	F			= 175,
	FS		= 185,
	G			= 196,
	GS		= 208,
	A			= 220,
	AS		= 233,
	B			= 247
};

enum class Octave
{
	C3 = 0,
	C4 = 1,
	C5 = 2,
	C6 = 3,
	C7 = 4,
	C8 = 5
};

#define NOTE_DUR_WHOLE					4.0f
#define NOTE_DUR_HALF						2.0f
#define NOTE_DUR_QUARTER				1.0f
#define NOTE_DUR_EIGHTH					0.5f
#define NOTE_DUR_SIXTEENTH			0.25f
#define NOTE_DUR_THIRTY_SECOND	0.125f

/* Pause between two notes [ms] */
#define NOTE_PAUSE	50

/* Struct for a tone */
typedef struct
{
	Note note;
	Octave octave;
	float duration;
}tone_t;

#endif
