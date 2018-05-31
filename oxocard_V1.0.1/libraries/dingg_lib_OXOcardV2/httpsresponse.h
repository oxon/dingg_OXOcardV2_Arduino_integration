#ifndef _LIB_HTTPS_RESPONSE_H_
#define _LIB_HTTPS_RESPONSE_H_
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
#include <stdint.h>
#include "wifitlsresponse.h"

class HTTPSResponse : public WifiTLSResponse{

	public:
		HTTPSResponse();
		~HTTPSResponse();

		int8_t start();
		int8_t read(size_t amount);
		int8_t stop();

		size_t handleHeader(size_t start, size_t amount);
		virtual void prepareBody();
		virtual size_t handleBody(size_t start, size_t amount);

		uint16_t getStatusCode(void);
		char *getBody();

		char* getBuffer();
		uint32_t getBufferLength();

	protected:



		uint8_t parsingHeader = 0;
		uint8_t parsingBody = 0;

		uint16_t statusCode = 0;

		static const uint16_t MAX_BODY_LENGTH = 4096;
		char *body = NULL;
		uint8_t hasBodyLength = 0;
		uint32_t bodyLength = 0;
		uint16_t currentBodyIndex;

		static const uint16_t MAX_BUFFER_LENGTH = 4096;
		char buffer[HTTPSResponse::MAX_BUFFER_LENGTH] = {0};

	private:
		const char *LOG_TAG = "HTTPSResponse";

};

#endif
