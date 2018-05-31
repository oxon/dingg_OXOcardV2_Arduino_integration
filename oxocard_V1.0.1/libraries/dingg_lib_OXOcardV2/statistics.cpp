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
#include "statistics.h"

Statistics::Statistics(){

}

Statistics::~Statistics(){

}

void Statistics::increaseCounter(const char *name){
PersistentMemory pm = PersistentMemory("statistics", PersistentMemory::WRITE_MODE);
	uint32_t currentCounter = 0;
	pm.getUInt32(name, &currentCounter);
	pm.saveUInt32(name, currentCounter+1);
}

int8_t Statistics::checkCounter(const char *name, uint32_t *value){
PersistentMemory pm = PersistentMemory("statistics");
	return pm.getUInt32(name, value);

}

uint32_t Statistics::getCounter(const char *name){
PersistentMemory pm = PersistentMemory("statistics");
	uint32_t value=0;
	pm.getUInt32(name, &value);
	return value;
}
