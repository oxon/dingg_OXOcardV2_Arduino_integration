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
#include "wifitls.h"

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

WifiTLS::WifiTLS(){

}

WifiTLS::~WifiTLS(){
	this->disconnect();
}

int8_t WifiTLS::setServerPort(int16_t port){
	this->serverPort = port;
	return 0;
}

int8_t WifiTLS::setServerHostname(char *hostname){
	this->serverHostname = hostname;
	return 0;
}

int8_t WifiTLS::connect(){
	int8_t initResult = this->init();
	if (initResult) {
		ESP_LOGE(this->LOG_TAG, "connect: failed to initialise the module context");
		return -1;
	}

	mbedtls_net_init(&this->serverFD);

	char portBuf[6];
	sprintf(portBuf, "%d", this->serverPort);

	int connectResult = mbedtls_net_connect(&this->serverFD, this->serverHostname, portBuf, MBEDTLS_NET_PROTO_TCP);
	if (connectResult != 0) {
		ESP_LOGE(this->LOG_TAG, "connect: failed to connect to server! rason: %i", connectResult);
		this->reset();
		return -2;
	}

	ESP_LOGD(this->LOG_TAG, "connect: connected to server '%s', fd = %d", this->serverHostname, this->serverFD.fd);

	// WORKAROUND
	// http://www.esp32.com/viewtopic.php?f=14&t=1007
	vTaskDelay(100 / portTICK_PERIOD_MS);

	// Define input and output functions for sending and receiving network data.
	mbedtls_ssl_set_bio(&this->sslContext, &this->serverFD, mbedtls_net_send, mbedtls_net_recv, NULL);

	ESP_LOGD(this->LOG_TAG, "connect: starting handshake");
	int handshakeResult = this->handshake();
	if (handshakeResult != 0) {
		ESP_LOGE(this->LOG_TAG, "connect: handshake failed");
		this->disconnect();
		return -3;
	}

	//TODO
    // Verify Root CA Certificate

   /*
    uint32_t caCertVerificationResult = mbedtls_ssl_get_verify_result(&ctx->ssl);
    if (caCertVerificationResult != 0) {
        wifi_tls_print_mbedtls_error((char *)("wifi_tls_connect: mbedtls_ssl_get_verify_result"), caCertVerificationResult);
        wifi_tls_disconnect(ctx);
        return -1;
    }

    // Verify Peer Certificate (Certificate Pinning)

    int pinningResult = wifi_tls_cert_pinning(ctx);
    if (pinningResult != 0) {
        ESP_LOGE(TAG, "wifi_tls_connect: certificate pinning failed");
        wifi_tls_disconnect(ctx);
        return -1;
    }*/

	ESP_LOGI(this->LOG_TAG, "Started valid TLS/SSL session with server '%s'.", this->serverHostname);
	return 0;
}

int8_t WifiTLS::disconnect(){
	if(this->connected == 0) return 0;
	mbedtls_net_free(&this->serverFD);
	this->reset();
	return 0;
}

int8_t WifiTLS::validate(){
	if(!this->serverPort || !this->serverHostname){
		ESP_LOGE(this->LOG_TAG, "Not sufficent host information provided.");
		return -1;
	}

	// TODO validate public-keys
	return 0;
}

int8_t WifiTLS::handshake(){
	while (1) {
		int handshakeResult = mbedtls_ssl_handshake(&this->sslContext);
		if (handshakeResult == 0) {
			break;
		}

		if (handshakeResult == MBEDTLS_ERR_SSL_WANT_READ || handshakeResult == MBEDTLS_ERR_SSL_WANT_WRITE){
			continue;
		}

		ESP_LOGE(this->LOG_TAG, "handshake: handshake failed! reason: %i", handshakeResult);
		return -1;
	}

	return 0;
}

int8_t WifiTLS::init(){
	if(this->validate() < 0){
		ESP_LOGE(this->LOG_TAG, "Couldn't validate info we have.");
		return -1;
	}

	mbedtls_ssl_init(&this->sslContext);
	mbedtls_x509_crt_init(&this->rootCACert);
	mbedtls_x509_crt_init(&this->peerCert);
	mbedtls_ctr_drbg_init(&this->ctrDrbgContext);
	mbedtls_ssl_config_init(&this->sslConf);
	mbedtls_entropy_init(&this->entropyContext);

	// Random number generator.
	int drbgSeedResult = mbedtls_ctr_drbg_seed(&this->ctrDrbgContext, mbedtls_entropy_func, &this->entropyContext, NULL, 0);
	if (drbgSeedResult != 0) {
		ESP_LOGE(this->LOG_TAG, "mbed_ctr_drbg_seed failed! rason: %i", drbgSeedResult);
		this->reset();
		return -2;
	}

	//TODO certs!
	// Root CA certificate.
	/*size_t buf_len = strlen(ctx->server_root_ca_public_key_pem) + 1; // needs to include the trailing 0x00!
	int cert_parse_result = mbedtls_x509_crt_parse(&ctx->root_ca_cert, (const unsigned char *)ctx->server_root_ca_public_key_pem, buf_len);
	if (cert_parse_result != 0) {
		wifi_tls_print_mbedtls_error((char *)("wifi_tls_init_context: mbedtls_x509_crt_parse failed for Root CA Cert"), cert_parse_result);
		wifi_tls_reset_context(ctx);
		return -1;
	}*/

	// Peer certificate (for certificate pinning).
	/*buf_len = strlen(ctx->peer_public_key_pem) + 1; // needs to include the trailing 0x00!
	cert_parse_result = mbedtls_x509_crt_parse(&ctx->peer_cert, (const unsigned char *)ctx->peer_public_key_pem, buf_len);
	if (cert_parse_result != 0) {
		wifi_tls_print_mbedtls_error((char *)("wifi_tls_init_context: mbedtls_x509_crt_parse failed for Peer Cert"), cert_parse_result);
		wifi_tls_reset_context(ctx);
		return -1;
	}*/

	// SSL configuration shared between SSL context structures.
	mbedtls_ssl_config_defaults(&this->sslConf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	//mbedtls_ssl_conf_authmode(&ctx->ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_authmode(&this->sslConf, MBEDTLS_SSL_VERIFY_NONE);
	//mbedtls_ssl_conf_ca_chain(&ctx->ssl_conf, &ctx->root_ca_cert, NULL);
	mbedtls_ssl_conf_rng(&this->sslConf, mbedtls_ctr_drbg_random, &this->ctrDrbgContext);

	// SSL Context
	mbedtls_ssl_set_hostname(&this->sslContext, this->serverHostname);
	int sslSetupResult = mbedtls_ssl_setup(&this->sslContext, &this->sslConf);
	if (sslSetupResult) {
		ESP_LOGE(this->LOG_TAG, "mbedtls_ssl_stup failed! rason: %i", sslSetupResult);
		this->reset();
		return -3;
	}

	this->connected = 1;
	return 0;
}

int8_t WifiTLS::sendAndReceive(WifiTLSRequest *request, WifiTLSResponse *response){
	uint32_t lengthRemaining = request->getBufferLength();
	char *currentPosition = request->getBuffer();

	while(lengthRemaining > 0){
		int ret = mbedtls_ssl_write(&this->sslContext, (unsigned char *)currentPosition, lengthRemaining);
		if(ret > 0){
			lengthRemaining -= ret;
			currentPosition += ret;
			continue;
		}
		if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
			continue;
		}
		if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			continue;
		}
		ESP_LOGE(this->LOG_TAG, "sendAndReceive: write: error, disconnecting, context is invalid: %i", ret);
		this->disconnect();
		return -1;
	}

	response->callbackIndex = 0;
	response->start();

	while(1){
		int ret = mbedtls_ssl_read(&this->sslContext, (unsigned char *) response->getBuffer(), response->getBufferLength());
		if(ret == 0){
			this->disconnect();
			return 0;
		}

		if(ret > 0){
			int8_t shouldContinue = response->read(ret);
			if(shouldContinue != 1){
				this->disconnect();
				return 0;
			}
			response->callbackIndex++;
			continue;
		}
		if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
			continue;
		}
		if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			continue;
		}
		ESP_LOGE(this->LOG_TAG, "sendAndReceive: read: error, disconnecting, context is invalid: %i", ret);
		this->disconnect();
		return -1;
	}

	return 0;
}

int8_t WifiTLS::reset(){
	mbedtls_entropy_free(&this->entropyContext);
	mbedtls_ssl_config_free(&this->sslConf);
	mbedtls_ctr_drbg_free(&this->ctrDrbgContext);
	//TODO apply when certs are implemented.
	//mbedtls_x509_crt_free(&this->rootCAcert);
	//mbedtls_x509_crt_free(&this->peerCert);
	mbedtls_ssl_free(&this->sslContext);

	ESP_LOGI(this->LOG_TAG, "Reset for server: %s", this->serverHostname);
	this->connected = 0;
	return 0;
}
