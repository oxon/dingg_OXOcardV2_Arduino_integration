#ifndef _LIB_IAP_H_
#define _LIB_IAP_H_
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

 //
//  iap_https.h
//  esp32-ota-https
//
//  Updating the firmware over the air.
//
//  This module is responsible to trigger and coordinate firmware updates.
//
//  Created by Andreas Schweizer on 11.01.2017.
//  Copyright © 2017 Classy Code GmbH
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <esp_ota_ops.h>
#include <esp_system.h>
#include <stdint.h>

class IAP{

	public:
		IAP();
		~IAP();
		int8_t write(uint8_t *bytes, uint16_t lenght);
		int8_t init();
		int8_t abort();
		int8_t commit();

	protected:

	private:
		const char *LOG_TAG = "IAP";
		static const size_t PAGE_SIZE = 4096;
		uint8_t moduleStateFlags;
		const esp_partition_t *partition_to_program;

		uint16_t pageBufferIndex;
		uint8_t *pageBuffer;
		const esp_partition_t *partitionToFlash;
		uint32_t currentFlashAddress;
		esp_ota_handle_t OTAHandle;
		uint8_t opened;


		int8_t finish(uint8_t commit);
		int8_t writePageBuffer();
		int8_t findNextBootPartition();

};

#endif
