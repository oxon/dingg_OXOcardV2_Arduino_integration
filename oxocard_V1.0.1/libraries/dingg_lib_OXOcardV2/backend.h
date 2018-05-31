#ifndef _LIB_BACKEND_H_
#define _LIB_BACKEND_H_ 1
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

#include <esp_system.h>
#include <stdarg.h>
#include <stdint.h>
#include "httpsclient.h"

class Backend{
	public:
		const char *protocol = "https://";
		char hostname[64] = {0};
		char path_prefix[32] = {0};
		uint16_t port = 0;

		Backend();
		~Backend();

		int8_t flash();
		char *getWeather(const char *townid);
		char *getTime();
		char *registerCard();
		char *initPair();
		char *info();
		char *log(char *logs, size_t length);
		char *craftURL(const char *fmt, va_list list);
		char *craftURL(const char *fmt, ...);

	protected:

	private:
		const char *LOG_TAG = "Backend";
		HTTPSClient *client = NULL;

		HTTPSResponse *doGETRequest(const char *fmt, ...);
		HTTPSResponse *doPOSTRequest(const char *postBody, size_t postBodyLength, const char *fmt, ...);

		size_t baseURLLength = 0;
		char *baseURL = NULL;
};

#endif
