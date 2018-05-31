#ifndef _LIB_WIFI_TLS_H_
#define _LIB_WIFI_TLS_H_
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

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"

#include "wifitlsrequest.h"
#include "wifitlsresponse.h"

class WifiTLS{

	public:
		WifiTLS();
		~WifiTLS();
		int8_t connect();
		int8_t disconnect();
		int8_t setServerPort(int16_t);
		int8_t setServerHostname(char *);
		int8_t sendAndReceive(WifiTLSRequest *request, WifiTLSResponse *response);

	protected:


	private:
		const char *LOG_TAG = "WifiTLS";

		uint8_t connected = 0;

		char *serverHostname;
		int16_t serverPort;
		char *serverRootCaPublicKey;
		char *peerPublicKey;

		mbedtls_ssl_context sslContext;
		mbedtls_ssl_config sslConf;
		mbedtls_ctr_drbg_context ctrDrbgContext;
		mbedtls_entropy_context entropyContext;
		mbedtls_x509_crt rootCACert;
		mbedtls_x509_crt peerCert;
		mbedtls_net_context serverFD =  {};

		int8_t handshake();
		int8_t validate();
		int8_t init();
		int8_t reset();


};

#endif
