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

#include <Arduino.h>
#include <esp_log.h>
#include <string.h>
#include <WiFiClientSecure.h>
#include "images.h"
#include "system.h"
#include "wificonfiguration.h"
#include "wireless.h"

int8_t Wireless::connected = 0;
const char* Wireless::WIFI_EVENTS[] = {
	"SYSTEM_EVENT_WIFI_READY",
	"SYSTEM_EVENT_SCAN_DONE",
	"SYSTEM_EVENT_STA_START",
	"SYSTEM_EVENT_STA_STOP",
	"SYSTEM_EVENT_STA_CONNECTED",
	"SYSTEM_EVENT_STA_DISCONNECTED",
	"SYSTEM_EVENT_STA_AUTHMODE_CHANGE",
	"SYSTEM_EVENT_STA_GOT_IP",
	"SYSTEM_EVENT_STA_WPS_ER_SUCCESS",
	"SYSTEM_EVENT_STA_WPS_ER_FAILED",
	"SYSTEM_EVENT_STA_WPS_ER_TIMEOUT",
	"SYSTEM_EVENT_STA_WPS_ER_PIN",
	"SYSTEM_EVENT_AP_START",
	"SYSTEM_EVENT_AP_STOP",
	"SYSTEM_EVENT_AP_STACONNECTED",
	"SYSTEM_EVENT_AP_STADISCONNECTED",
	"SYSTEM_EVENT_AP_PROBEREQRECEVED",
	"SYSTEM_EVENT_AP_STA_GIT_IP6"
};

std::list<WifiConfiguration> Wireless::storedWifis = {};
std::list<WifiConfiguration> Wireless::availableWifis = {};

Wireless::Wireless(){ }

Wireless::~Wireless(){ }

void Wireless::init(){
	ESP_LOGI(this->LOG_TAG, "Initializing");

	Wireless::loadWifis();

	WiFi.onEvent(Wireless::eventCallback);
}

void Wireless::loadWifis(){
	PersistentMemory memory = PersistentMemory("wireless", PersistentMemory::READ_ONLY);

	char skey[8] = "ssid0";
	char pkey[12] = "password0";
	WifiConfiguration current = WifiConfiguration();
	int index = 0;

	Wireless::storedWifis.clear();

	ESP_LOGI("Static Wifi", "Loading saved wifis.");
	while(memory.getStr(skey, current.ssid, Wireless::SSID_MAX_LENGTH+1) > 0){
		if(memory.getStr(pkey, current.password, Wireless::PASSWORD_MAX_LENGTH+1) > 0){
			Wireless::storedWifis.push_back(current);
		}
		index++;
		snprintf(skey, 8, "ssid%i", index);
		snprintf(pkey, 12, "password%i", index);
		current = WifiConfiguration();
	}
}

int8_t Wireless::isConnected(){
	return Wireless::connected;
}

int8_t Wireless::storeWifi(const char *ssid, const char* password){
	ESP_LOGI("Static Wifi", "Storing wifi %s", ssid);

	PersistentMemory *memory = new PersistentMemory("wireless", PersistentMemory::WRITE_MODE);
	char skey[8] = "ssid0";
	char pkey[12] = "password0";

	uint8_t currentIndex = 0;
	int16_t foundWifiIndex = -1;
	for(auto swifi = Wireless::storedWifis.begin(); swifi != Wireless::storedWifis.end(); ++swifi) {
		if(strncmp(ssid, (*swifi).ssid, strlen(ssid)) == 0 && strlen(ssid) == strlen((*swifi).ssid)){
			foundWifiIndex = currentIndex;
		}
		if(foundWifiIndex > -1) break;
		currentIndex++;
	}
	if(foundWifiIndex > -1){
		snprintf(skey, 8, "ssid%i", foundWifiIndex);
		snprintf(pkey, 12, "password%i", foundWifiIndex);
		if(memory->saveStr((const char *)skey, ssid)) return -1;
		if(memory->saveStr((const char *)pkey, password)) return -2;
		return 0;
	}

	int index = 0;
	WifiConfiguration current = WifiConfiguration();
	while(memory->getStr(skey, current.ssid, Wireless::SSID_MAX_LENGTH+1) > 0){
		if(memory->getStr(pkey, current.password, Wireless::PASSWORD_MAX_LENGTH+1) > 0){
			index++;
			snprintf(skey, 8, "ssid%i", index);
			snprintf(pkey, 12, "password%i", index);
		}
	}

	if(memory->saveStr((const char *)skey, ssid)) return -3;
	if(memory->saveStr((const char *)pkey, password)) return -4;

	Wireless::loadWifis();

	return 0;
}

int8_t Wireless::deleteWifi(uint8_t deleteIndex){

	ESP_LOGI("Static Wifi", "Deleting wifi %d", deleteIndex);

	PersistentMemory *memory = new PersistentMemory("wireless", PersistentMemory::WRITE_MODE);
	char fromSKey[8] = "ssid0";
	char fromPKey[12] = "password0";
	char toSKey[8] = "ssid0";
	char toPKey[12] = "password0";

	WifiConfiguration nextWifi = WifiConfiguration();
	uint8_t found = 0;
	uint8_t currentIndex = 0;
	for(auto swifi = Wireless::storedWifis.begin(); swifi != Wireless::storedWifis.end(); ++swifi) {
		if(currentIndex == deleteIndex){
			found = 1;
		}
		if(found == 0){currentIndex++; continue;}

		snprintf(fromSKey, 8, "ssid%i", currentIndex+1);
		snprintf(fromPKey, 12, "password%i", currentIndex+1);
		ESP_LOGI("Static Wifi", "Loading %s  and %s ", fromSKey, fromPKey);
		if(memory->getStr(fromSKey, nextWifi.ssid, Wireless::SSID_MAX_LENGTH+1) > 0){
			if(memory->getStr(fromPKey, nextWifi.password, Wireless::SSID_MAX_LENGTH+1) > 0){
				snprintf(toSKey, 8, "ssid%i", currentIndex);
				snprintf(toPKey, 12, "password%i", currentIndex);
				ESP_LOGI("Static Wifi", "Replacing %s  with %s (%s)", toSKey, fromSKey , nextWifi.ssid);
				if(memory->saveStr(toSKey, nextWifi.ssid)) return -3;
				if(memory->saveStr(toPKey, nextWifi.password)) return -4;
			}
		}else{
			snprintf(toSKey, 8, "ssid%i", currentIndex);
			snprintf(toPKey, 12, "password%i", currentIndex);
			memory->remove(toSKey);
			memory->remove(toPKey);
		}
		currentIndex++;
	}
	Wireless::loadWifis();
	return 0;
}

int8_t Wireless::autoConnectWithoutRetries(){
	if(this->isConnected())
		return 0;
	ESP_LOGI(this->LOG_TAG, "trying to connect.");
	this->tryToConnect();
	if(this->isConnected())
		return 0;
	ESP_LOGI(this->LOG_TAG, "We couldn't connect.");
	return -1;
}

int8_t Wireless::autoConnect(){
	for(uint8_t i=0; i<Wireless::MAX_AUTO_CONNECT_RETRIES; i++){
		this->autoConnectWithoutRetries();
		if(this->isConnected()) return 0;
		else this->forceConnect();
	}
	return -1;
}

int8_t Wireless::autoConnectWithFeedback(Matrix *matrix){
	int8_t result = this->autoConnect();
	if(result == 0) return 0;
	for(uint8_t i=0; i<20; i++){
		matrix->clear();
		matrix->drawImage(imageWlanIcon, 0, 0, oxocardBrandColors[3]);
		vTaskDelay(200/portTICK_PERIOD_MS);
		matrix->clear();
		matrix->update();
		vTaskDelay(200/portTICK_PERIOD_MS);
	}
	return result;
}

int8_t Wireless::forceConnect(){
	ESP_LOGI(this->LOG_TAG, "Brute-Force to maybe connect to hidden-ssid.");
	for(size_t i=0; i<Wireless::storedWifis.size(); i++){
		this->connectToNetwork(i);
		if(Wireless::connected)
			return 0;
	}
	return -1;
}

int8_t Wireless::tryToConnect(){
	this->scan();

	ESP_LOGI(this->LOG_TAG, "We have to check %d wifis.", Wireless::storedWifis.size());

	size_t currentIndex = 0;

	for(auto swifi = Wireless::storedWifis.begin(); swifi != Wireless::storedWifis.end(); ++swifi) {
		for(auto awifi = Wireless::availableWifis.begin(); awifi != Wireless::availableWifis.end(); ++awifi) {
			if(strncmp((*awifi).ssid, (*swifi).ssid, strlen((*swifi).ssid)) == 0 && strlen((*awifi).ssid) == strlen((*swifi).ssid)){
				ESP_LOGI(this->LOG_TAG, "Wifi '%s' seems to be in range. Trying to connect", (*awifi).ssid);
				this->connectToNetwork(currentIndex);
				if(Wireless::connected)
					return 0;
			}
		}
		currentIndex++;
	}
	return -3;
}

int8_t Wireless::connectToNetwork(uint8_t index){
	return this->connectToNetwork(index, Wireless::DEFAULT_CONNECT_TIMEOUT_IN_S);
}

int8_t Wireless::connectToNetwork(uint8_t index, uint8_t timeout){
	if(Wireless::storedWifis.size() < 1 || index > storedWifis.size()){
		ESP_LOGW(this->LOG_TAG, "Cannot connect to network. Either no networks saved or invalid index provided.");
		return -1;
	}

	WiFi.disconnect(true);
	WiFi.mode(WIFI_MODE_STA);
	vTaskDelay(500/portTICK_PERIOD_MS); //TODO bugfix?!

	std::list<WifiConfiguration>::iterator it = Wireless::storedWifis.begin();
	std::advance(it, index);
	WifiConfiguration network = (WifiConfiguration)*it;
	ESP_LOGI(this->LOG_TAG, "Connecting to network: %s", network.ssid);
	WiFi.begin(network.ssid, network.password);

	uint16_t waited = 0;
	while(!Wireless::connected){
		vTaskDelay(50/portTICK_PERIOD_MS);
		waited ++;
		if(waited/20 > timeout)
			return -1;
	}

	return 0;
}

int8_t Wireless::testConnectivity(WifiConfiguration wc){
	return Wireless::testConnectivity(wc, Wireless::DEFAULT_CONNECT_TIMEOUT_IN_S);
}


int8_t Wireless::testConnectivity(WifiConfiguration wc, uint8_t timeoutInS){
	ESP_LOGI("Wifi Static", "Testing connectivity for %s", wc.ssid);

	WiFi.disconnect();
	Wireless::connected = 0;

	WiFi.enableSTA(true);
	WiFi.begin(wc.ssid, wc.password);

	uint16_t waited = 0;
	while(!Wireless::connected){
		vTaskDelay(50/portTICK_PERIOD_MS);
		waited ++;
		if(waited/20 > timeoutInS){
			return -1;
		}
	}

	WiFi.disconnect();
	WiFi.enableSTA(false);
	return 0;
}

int8_t Wireless::startAP(){
	WiFi.mode(WIFI_MODE_APSTA);

	if(!WiFi.enableAP(true)) {
		return -1;
	}

	tcpip_adapter_ip_info_t info;
	info.ip.addr = static_cast<uint32_t>(this->accessPointIP);
	info.gw.addr = static_cast<uint32_t>(this->accessPointIP);
	info.netmask.addr = static_cast<uint32_t>(IPAddress(255, 255, 255, 0));
	tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);

	if(!(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info) == ESP_OK)){
		ESP_LOGW(this->LOG_TAG, "we couldn't set IP info. not starting AP.");
		return -2;
	}


	dhcps_lease_t lease;
	lease.enable = true;
	lease.start_ip.addr = static_cast<uint32_t>(this->accessPointIP) + (1 << 24);
	lease.end_ip.addr = static_cast<uint32_t>(this->accessPointIP) + (11 << 24);
	tcpip_adapter_dhcps_option(
		(tcpip_adapter_option_mode_t)TCPIP_ADAPTER_OP_SET,
		(tcpip_adapter_option_id_t)TCPIP_ADAPTER_REQUESTED_IP_ADDRESS,
		(void*)&lease, sizeof(dhcps_lease_t)
	);

	ESP_LOGI(this->LOG_TAG, "we have dns IP: %d", static_cast<uint32_t>(this->accessPointIP));

	tcpip_adapter_dns_info_t dns_info;
	dns_info.ip.u_addr.ip4.addr =static_cast<uint32_t>(this->accessPointIP);

	tcpip_adapter_dns_param_t dnsdhcp = {TCPIP_ADAPTER_DNS_MAIN, &dns_info};

	tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_AP, TCPIP_ADAPTER_DNS_MAIN, &dns_info);

	tcpip_adapter_dhcps_option(
		(tcpip_adapter_option_mode_t)TCPIP_ADAPTER_OP_SET,
		(tcpip_adapter_option_id_t)TCPIP_ADAPTER_DOMAIN_NAME_SERVER,
		(void*)&dnsdhcp, sizeof(dnsdhcp)
	);

	tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);

	WiFi.softAP(System::getName());
	this->APRunning = 1;

	ESP_LOGI(this->LOG_TAG, "AP started.");
	return 0;

}

int8_t Wireless::stopAP(){
	WiFi.softAPdisconnect();
	WiFi.enableAP(false); // disable
	return 0;
}

void Wireless::scan(uint16_t msPerChannel){
	int amount = WiFi.scanNetworks(false, false, false, msPerChannel);
	ESP_LOGI(this->LOG_TAG, "Found %i networks.", amount);
	WifiConfiguration current = WifiConfiguration();
	int8_t found = 0;
	for(int i=0; i<amount; i++){
		WiFi.SSID(i).toCharArray(current.ssid, Wireless::SSID_MAX_LENGTH+1);
		found = 0;
		for(auto saved = this->availableWifis.begin(); saved != this->availableWifis.end(); ++saved) {
			if( strncmp((*saved).ssid, current.ssid, strlen((*saved).ssid)) == 0) found = 1;
		}

		if(found == 0){
			Wireless::availableWifis.push_back(current);
			current = WifiConfiguration();
		}
	}
}

void Wireless::scan(void){
	this->scan(Wireless::DEFAULT_SCAN_TIME_PER_CHANNEL_IN_MS);
}

void Wireless::eventCallback(WiFiEvent_t event){
	ESP_LOGI("Static Wifi", "WifiEvent: %s", Wireless::WIFI_EVENTS[event]);
	if(event == SYSTEM_EVENT_STA_GOT_IP){
		IPAddress a = WiFi.localIP();
		ESP_LOGI("Static Wifi", "Connected to network with IP: %i.%i.%i.%i", a[0], a[1], a[2], a[3]);
		Wireless::connected = 1;
	}
}
