#ifndef _LIB_BACKEND_LOG_H_
#define _LIB_BACKEND_LOG_H_ 1
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
#include <esp_system.h>
#include <stdint.h>
#include "backend.h"

class BackendLog{

	public:
		static BackendLog &getInstance(){
			static BackendLog instance;
			return instance;
		}

		void activate();
		void deactivate();

		vprintf_like_t defaultLogger = NULL;
		void setLog(const char *fmt , va_list list);

		BackendLog(BackendLog const&) = delete;
		void operator=(BackendLog const&) = delete;

	protected:

	private:
		const static size_t BUFFER_SIZE = 2048;

		BackendLog();
		~BackendLog();

		const char *LOG_TAG = "BackendLog";

		void upload(size_t length);

		size_t bufferIndex = 0;
		char *buffer = NULL;
		Backend *backend = NULL;
};

#endif
