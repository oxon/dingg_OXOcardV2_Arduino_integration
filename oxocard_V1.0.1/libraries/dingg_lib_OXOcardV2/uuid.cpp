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
#include "random.h"
#include "uuid.h"

UUID::UUID(){

}

UUID::~UUID(){

}

uint8_t UUID::generateV4(char *destination){
	for(int i=0; i<UUID::LENGTH; i++){
		if( i==8 || i==13 || i==18 || i==23){
			destination[i] = '-';
			continue;
		}
		uint8_t byte = Random::getByte();
		if(i == 14){
			byte &= 0b00001111;
			byte |= 1 << 6;

		}
		if(i==19){
			byte &= 0b00111111;
			byte |= 1 << 7;
		}
		destination[i] = this->toHex(byte >> 4);
		destination[i+1] = this->toHex(byte&0b00001111);
		i++;
	}
	destination[UUID::LENGTH] = '\0';

	return 0;
}

char UUID::toHex(uint8_t value){
	if(value > 15) return '\0';
	if(value < 10) return value + 48;
	return value + 87;
}
