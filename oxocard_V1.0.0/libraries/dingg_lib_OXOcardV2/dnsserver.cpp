#include <esp_log.h>
#include <lwip/def.h>
#include "dnsserver.h"

DNSEntry::DNSEntry(const char* name, IPAddress a){
	strncpy(this->name, name, sizeof(this->name));
	for(size_t i=0; i<4; i++){
		this->ipAddress[i] = a[i];
	}
}

DNSServer::DNSServer(){
	this->port = 53;
}

int8_t DNSServer::start(){
	return this->udp.begin(this->port) == 1;
}

void DNSServer::addDNSEntry(DNSEntry &entry){
	this->entries.push_back(entry);
}

void DNSServer::stop(){
	this->udp.stop();

}

void DNSServer::processNextRequest(){
	size_t packetSize = this->udp.parsePacket();
	if (packetSize > 0){
		unsigned char buffer[packetSize] = {0};

		this->udp.read(buffer, packetSize);
		DNSHeader header = {};

		memcpy(&header, buffer, sizeof(header));

		ESP_LOGI(this->LOG_TAG, "query with ID: %d", ntohs(header.ID));
		if(ntohs(header.QR) != DNS_QR_QUERY || ntohs(header.OPCode) != DNS_OPCODE_QUERY){
			ESP_LOGW(this->LOG_TAG, "We have not a query or not implemented OPCODE.");
			this->failRequest(header.ID);
		}

		if(ntohs(header.QDCount) == 1){
			char hostname[128] = {0};
			size_t hostnameIndex = 0;
			unsigned char *qname = (unsigned char *)buffer+sizeof(header);
			size_t position = 0;
			while(*(qname + position) != 0){
				size_t octetLength = *(qname + position);
				position++;
				if(hostnameIndex+octetLength < 128){
					memcpy(hostname+hostnameIndex, (qname + position), octetLength);
					hostnameIndex += octetLength;
					hostname[hostnameIndex] = '.';
					hostnameIndex ++;
				}
				position += octetLength;
			}
			ESP_LOGI(this->LOG_TAG, "hostname: %s", hostname);
			int8_t replied = 0;
			for(auto entry = this->entries.begin(); entry != this->entries.end(); ++entry) {
				DNSEntry e = *entry;
				if(strncmp(hostname, e.name, (hostnameIndex-1)) == 0 && strlen(e.name) == strlen(hostname)-1){
					this->replyWithIP(header, qname, position+5,e.ipAddress);
					replied = 1;
				}
			}
			if(!replied){
				ESP_LOGW(this->LOG_TAG, "Invalid host requested. failing request.");
				this->failRequest(header.ID);
			}
		}else{
			ESP_LOGW(this->LOG_TAG, "We have invalid counts, that we don't support. QD: %d, AN: %d, NS: %d, AR: %d", ntohs(header.QDCount), ntohs(header.ANCount), ntohs(header.NSCount), ntohs(header.ARCount));
			this->failRequest(header.ID);
		}
	}
}

void DNSServer::failRequest(uint16_t id){
	DNSHeader header = {};
	header.ID = id;
	header.QR = DNS_QR_RESPONSE;
	header.RCode = (unsigned char)DNSReplyCode::ServerFailure;
	header.QDCount = 0;

	unsigned char buffer[sizeof(header)];
	memcpy(buffer, &header, sizeof(header));
	this->udp.beginPacket(this->udp.remoteIP(), this->udp.remotePort());
	this->udp.write(buffer, sizeof(header));
	this->udp.endPacket();
}

void DNSServer::replyWithIP(DNSHeader requestHeader, unsigned char *requestDomain, size_t length, IPAddress ipaddress){
	this->udp.beginPacket(this->udp.remoteIP(), this->udp.remotePort());

	DNSHeader header = {};
	header.ID = requestHeader.ID;
	header.QR = DNS_QR_RESPONSE;
	header.QDCount = requestHeader.QDCount;
	header.ANCount = requestHeader.QDCount;
	header.RA = htons(0);
	header.RCode = (unsigned char)DNSReplyCode::NoError;


	this->udp.write((unsigned char*)&header, sizeof(header));

	this->udp.write(requestDomain, length);




	this->udp.write((uint8_t)192); //  answer name is a pointer
	this->udp.write((uint8_t)12);  // pointer to offset at 0x00c

	this->udp.write((uint8_t)0);   // 0x0001  answer is type A query (host address)
	this->udp.write((uint8_t)1);

	this->udp.write((uint8_t)0);   //0x0001 answer is class IN (internet address)
	this->udp.write((uint8_t)1);

	uint32_t ttl = htonl(60);
	this->udp.write((unsigned char*)&ttl, 4);

	// Length of RData is 4 bytes (because, in this case, RData is IPv4)
	this->udp.write((uint8_t)0);
	this->udp.write((uint8_t)4);
	unsigned char ipaddress_c[4];
	for(size_t i=0; i<4; i++)
		ipaddress_c[i] = ipaddress[i];
	this->udp.write(ipaddress_c, sizeof(ipaddress_c));
	this->udp.endPacket();

}


/*
String DNSServer::getDomainName()
{
  String parsedDomainName = "";

  unsigned char *start = _buffer + 12;

  int pos = 0;
  while(true)
  {
    unsigned char labelLength = *(start + pos);
    for(int i = 0; i < labelLength; i++)
    {
      pos++;
      parsedDomainName += (char)*(start + pos);
    }
    pos++;
    if (*(start + pos) == 0)
    {
      downcaseAndRemoveWwwPrefix(parsedDomainName);
      return parsedDomainName;
    }
    else
    {
      parsedDomainName += ".";
    }
  }
}*/
/*
void DNSServer::replyWithIP()
{
  if (_buffer == NULL) return;
  _dnsHeader->QR = DNS_QR_RESPONSE;
  _dnsHeader->ANCount = _dnsHeader->QDCount;
  _dnsHeader->QDCount = _dnsHeader->QDCount;
  //_dnsHeader->RA = 1;

  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.write(_buffer, _currentPacketSize);

  _udp.write((uint8_t)192); //  answer name is a pointer
  _udp.write((uint8_t)12);  // pointer to offset at 0x00c

  _udp.write((uint8_t)0);   // 0x0001  answer is type A query (host address)
  _udp.write((uint8_t)1);

  _udp.write((uint8_t)0);   //0x0001 answer is class IN (internet address)
  _udp.write((uint8_t)1);

  _udp.write((unsigned char*)&_ttl, 4);

  // Length of RData is 4 bytes (because, in this case, RData is IPv4)
  _udp.write((uint8_t)0);
  _udp.write((uint8_t)4);
  _udp.write(_resolvedIP, sizeof(_resolvedIP));
  _udp.endPacket();



  #ifdef DEBUG
    DEBUG_OUTPUT.print("DNS responds: ");
    DEBUG_OUTPUT.print(_resolvedIP[0]);
    DEBUG_OUTPUT.print(".");
    DEBUG_OUTPUT.print(_resolvedIP[1]);
    DEBUG_OUTPUT.print(".");
    DEBUG_OUTPUT.print(_resolvedIP[2]);
    DEBUG_OUTPUT.print(".");
    DEBUG_OUTPUT.print(_resolvedIP[3]);
    DEBUG_OUTPUT.print(" for ");
    DEBUG_OUTPUT.println(getDomainNameWithoutWwwPrefix());
  #endif
}

*/
