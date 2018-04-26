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
#include "httpsclient.h"

HTTPSClient::HTTPSClient(){

}

HTTPSClient::~HTTPSClient(){
	if(this->tls != NULL){
		delete this->tls;
		this->tls = NULL;
	}
}

HTTPSResponse *HTTPSClient::sendRequest(HTTPSRequest *request){
	if(this->tls != NULL) delete this->tls;

	this->tls = new WifiTLS();

	tls->setServerPort(request->getServerPort());
	tls->setServerHostname(request->getServerHostname());
	tls->connect();

	//WifiTLSRequest tlsRequest = WifiTLSRequest(request->getBuffer(), request->getBufferLength());

	HTTPSResponse *response = new HTTPSResponse();
	tls->sendAndReceive((WifiTLSRequest *)request, (WifiTLSResponse *)response);

	delete this->tls;
	this->tls = NULL;
	return response;
}
