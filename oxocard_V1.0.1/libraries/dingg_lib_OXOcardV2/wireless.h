#ifndef _LIB_WIFI_H_
#define _LIB_WIFI_H_ 1
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
#include <list>
#include <stdint.h>
#include <WiFi.h>
#include "accesspoint.h"
#include "matrix.h"
#include "persistentmemory.h"

/**
 * Description of the class
 */
class WifiConfiguration;
class Wireless{

	public:

		static const char *WIFI_EVENTS[];

		const static uint8_t SSID_MAX_LENGTH = 32;
		const static uint8_t PASSWORD_MAX_LENGTH = 32;
		 //there are 13 channels. so do not increase to high....
		const static uint16_t DEFAULT_SCAN_TIME_PER_CHANNEL_IN_MS = 400;
		const static uint8_t DEFAULT_CONNECT_TIMEOUT_IN_S = 12;
		const static uint8_t MAX_AUTO_CONNECT_RETRIES = 2;

		Wireless();
		~Wireless();

		void init();
		void scan();
		void scan(uint16_t msPerChannel);

		int8_t isConnected();
		int8_t autoConnectWithoutRetries();
		int8_t autoConnect();
		int8_t autoConnectWithFeedback(Matrix *matrix);

		int8_t startAP();
		int8_t stopAP();
		static int8_t testConnectivity(WifiConfiguration wc);
		static int8_t testConnectivity(WifiConfiguration wc, uint8_t timeoutInS);

		static int8_t storeWifi(const char *ssid, const char *password);
		static int8_t deleteWifi(uint8_t deleteIndex);
		static std::list<WifiConfiguration> storedWifis;
		static std::list<WifiConfiguration> availableWifis;
	protected:


	private:

		const IPAddress accessPointIP = IPAddress(192,168,4,1);

		const uint8_t waitForWiFiTimeout = 30;
		const uint8_t waitForClientTimeout = 10;

		const char *LOG_TAG = "WiFi";

		static int8_t connected;
		uint8_t APRunning;
		static void eventCallback(WiFiEvent_t event);

		static void loadWifis();
		int8_t tryToConnect();
		int8_t forceConnect();
		int8_t connectToNetwork(uint8_t index);
		int8_t connectToNetwork(uint8_t index, uint8_t timeout);

};

#endif
