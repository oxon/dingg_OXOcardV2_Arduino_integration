#ifndef _LIB_STARTUP_H_
#define _LIB_STARTUP_H_ 1
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
#include "oxocard.h"

class Startup{

	public:
		Startup(OXOcard *oxocard);
		~Startup();
		void run();
		uint8_t isFirstBoot();

	protected:

	private:
		const char *LOG_TAG = "Startup";
		OXOcard *oxocard = NULL;
		PersistentMemory *memory = NULL;
		uint8_t firstBoot = 0;

		void printHelloMessage();
		int8_t initNVS();
		int8_t initMatrix();
		int8_t checkDeviceId();
		int8_t checkDeviceName();
		int8_t initializeSettings();
		int8_t checkBackendLogging();
		void drawBootImage(void);
};

#endif
