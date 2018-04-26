#ifndef _LIB_COMMUNICATION_H_
#define _LIB_COMMUNICATION_H_ 1
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
#include <WiFi.h>
#include <WiFiUdp.h>
#include "cJSON.h"

namespace Communication{

	class LocalMessage{

		static const char *PREFIX;

		public:
			LocalMessage(const char *message);
			LocalMessage(const char *sender, const char *userMessage, const char *receiver, uint32_t userNumber);
			LocalMessage(const char *sender, const char *userMessage, const char *receiver);
			LocalMessage(const char *sender, const char *userMessage);
			~LocalMessage();

			char *message = NULL;
			char *sender = NULL;
			char *receiver = NULL;
			char *userMessage = NULL;
			uint32_t userNumber = 0.0f;

		private:
			const char *LOG_TAG = "Communication::LocalMessage";
			int8_t parseMessage();
			int8_t craftMessage();
			int8_t initFromMessage(const char *message);
			int8_t initFromParams(const char *sender, const char *receiver, const char *userMessage, int32_t userNumber);
	};

	class Local{
		public:
			static const uint16_t UDP_PORT = 1337;

			Local();
			~Local();
			void start(const char *myName);
			void stop();
			int8_t reconfigure();

			int8_t waitForMessages();
			int8_t waitForMessages(uint16_t timeout);

			// DIRECT
			int8_t setDirectMessageFilter(const char *filterSender);
			int8_t isDirectMessageAndNotFiltered();
			int8_t sendDirectMessage(const char *message, const char *receiver);
			int8_t sendDirectNumber(int32_t number, const char *receiver);
			int8_t sendDirectMessageAndNumber(const char *message, int32_t number, const char *receiver);
			int8_t compareDirectMessageFrom(const char *message, const char *filterSender);
			int8_t compareDirectMessage(const char *message);
			int8_t compareDirectNumber(int32_t number);
			char *getDirectMessage();
			int32_t getDirectNumber();

			// BROADCAST
			int8_t sendBroadcastMessage(const char *message);
			int8_t sendBroadcastNumber(int32_t number);
			int8_t sendBroadcastMessageAndNumber(const char *message, int32_t number);
			int8_t isBroadcastMessage();
			int8_t compareBroadcastMessage(const char *message);
			int8_t compareBroadcastNumber(int32_t number);
			char *getBroadcastMessage();
			int32_t getBroadcastNumber();

		protected:

		private:
			const char *LOG_TAG = "Communication::Local";

			WiFiUDP *udp = NULL;
			uint8_t hasBegun = 0;

			const char *myName = NULL;
			char *filterSenderName = NULL;
			LocalMessage *message = NULL;

			IPAddress broadcast;

			void init();
			int8_t readPacket();
			int8_t writePacket(const char *receiver, const char *message, int32_t number);
	};
}

#endif
