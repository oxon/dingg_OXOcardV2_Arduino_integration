#ifndef _LIB_HTTP_SERVER_H_
#define _LIB_HTTP_SERVER_H_ 1
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
#include "httpmethod.h"
#include "cJSON.h"

class HTTPServer{

	public:
		HTTPServer();
		~HTTPServer();

		void parseRequest(const char *buffer, size_t length);
		void parseHeaderLine(const char *buffer, size_t length);
		void parseBody(const char *buffer, size_t length);
		char *getResponseHeader();
		size_t getResponseHeaderLength();
		char *getResponse();
		size_t getResponseLength();

	protected:


	private:
		const char *LOG_TAG = "HTTPServer";

		HTTPMethod method;
		char requestPath[16] = {0};
		size_t postDataLength = 0;
		char *postData;
		uint16_t statusCode = 200;

		char *responseHeader;
		size_t responseHeaderLength;
		char *response;
		size_t responseLength;

		char *contentType;

		uint8_t parsingHeader;
		void craftHeader();

		void processRequest();

		void serveIndex();
		void serveDebug();
		void serveError();
		void serveWifiList();
		void serveInfo();
		void serveSave();
		void serveSaveDebug();
		void serveDelete();
		void serveConnectivity();
		void serveFactoryReset();
};

#endif
