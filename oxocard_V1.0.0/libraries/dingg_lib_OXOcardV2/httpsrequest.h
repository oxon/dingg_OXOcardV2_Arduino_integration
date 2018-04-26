#ifndef _LIB_HTTPS_REQUEST_H_
#define _LIB_HTTPS_REQUEST_H_
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
#include <esp_system.h>
#include "httpmethod.h"
#include "wifitlsrequest.h"

class HTTPSRequest : public WifiTLSRequest{

	public:

		HTTPSRequest();
		~HTTPSRequest();
		HTTPSRequest(const char *url);
		int8_t reset();

		int8_t setURL(const char *url);
		int8_t setMethod(HTTPMethod method);
		void setPostBody(const char *body, size_t length);

		char *getServerHostname();
		char *getPath();
		uint16_t getServerPort();

		char* getBuffer();
		size_t getBufferLength();

	protected:


	private:
		const char *LOG_TAG = "HTTPSRequest";
		const uint8_t TIMEOUT = 30;

		HTTPMethod method = METHOD_GET;

		const char *url;
		uint16_t port;

		static const size_t MAX_HOSTNAME_LENGTH = 256;
		char hostname[HTTPSRequest::MAX_HOSTNAME_LENGTH];
		size_t hostnameLength = 0;

		static const size_t MAX_PATH_LENGTH = 256;
		char path[HTTPSRequest::MAX_PATH_LENGTH];
		size_t pathLength = 0;

		size_t bufferLength;
		char *buffer = NULL;

		const char *body;
		size_t bodyLength = 0;

		int8_t parseURL();

};

#endif
