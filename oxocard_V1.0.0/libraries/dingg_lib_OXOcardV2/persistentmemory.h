#ifndef _LIB_PERSISTENT_MEMORY_H_
#define _LIB_PERSISTENT_MEMORY_H_
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

#include <esp_err.h>
#include <esp_system.h>
#include <nvs.h>
#include <stdint.h>


class PersistentMemory{

	public:
		const static uint8_t READ_ONLY = 0;
		const static uint8_t WRITE_MODE = 1;

		PersistentMemory(const char *identifier);
		PersistentMemory(const char *identifier, uint8_t writeMode);
		~PersistentMemory();

		int8_t clear();
		int8_t remove(const char *key);
		int8_t saveByte(const char *key, uint8_t value);
		int8_t saveInt32(const char *key, int32_t value);
		int8_t saveUInt32(const char *key, uint32_t value);
		int8_t saveStr(const char *key, const char *value);
		int8_t saveBlob(const char *key, const void *value, size_t len);
		int8_t getByte(const char *key, uint8_t *value);
		int8_t getInt32(const char *key, int32_t *value);
		int8_t getUInt32(const char *key, uint32_t *value);
		int16_t getStr(const char *key, char *value, size_t len);
		int16_t getBlob(const char *key, void *value, size_t len);
	protected:

	private:

		const char *LOG_TAG = "PersistentMemory";
		const char *identifier;
		uint8_t writeMode;
		uint8_t initialized = 0;
		uint32_t nvsHandle;

		void constructor(const char *identifier, uint8_t writeMode);
		int8_t open();
		int8_t close();
		const char *get_error(esp_err_t err);
		const char *nvs_errors[13] = {
			"OTHER",
			"NOT_INITIALIZED",
			"NOT_FOUND",
			"TYPE_MISMATCH",
			"READ_ONLY",
			"NOT_ENOUGH_SPACE",
			"INVALID_NAME",
			"INVALID_HANDLE",
			"REMOVE_FAILED",
			"KEY_TOO_LONG",
			"PAGE_FULL",
			"INVALID_STATE",
			"INVALID_LENGHT"
		};
};

#endif
