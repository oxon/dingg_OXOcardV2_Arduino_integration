#ifndef _LIB_DNSSERVER_H_
#define _LIB_DNSSERVER_H_ 1

#include <esp_system.h>
#include <list>
#include <WiFiUdp.h>

#define DNS_QR_QUERY 0
#define DNS_QR_RESPONSE 1
#define DNS_OPCODE_QUERY 0

enum class DNSReplyCode{
	NoError = 0,
	FormError = 1,
	ServerFailure = 2,
	NonExistentDomain = 3,
	NotImplemented = 4,
	Refused = 5,
	YXDomain = 6,
	YXRRSet = 7,
	NXRRSet = 8
};

typedef struct DNSHeader{
	uint16_t ID;               // identification number
	unsigned char RD : 1;      // recursion desired
	unsigned char TC : 1;      // truncated message
	unsigned char AA : 1;      // authoritive answer
	unsigned char OPCode : 4;  // message_type
	unsigned char QR : 1;      // query/response flag
	unsigned char RCode : 4;   // response code
	unsigned char Z : 3;       // its z! reserved
	unsigned char RA : 1;      // recursion available
	uint16_t QDCount;          // number of question entries
	uint16_t ANCount;          // number of answer entries
	uint16_t NSCount;          // number of authority entries
	uint16_t ARCount;          // number of resource entries
} DNSHeader;

class DNSEntry{
	public:
		char name[128] = {0};
		IPAddress ipAddress = IPAddress(0,0,0,0);

		DNSEntry();
		DNSEntry(const char* name, IPAddress a);
};

class DNSServer{
	public:
		DNSServer();
		void processNextRequest();
		int8_t start();
		void stop();
		void failRequest(uint16_t id);
		void addDNSEntry(DNSEntry &entry);

	private:
		const char *LOG_TAG = "DNSServer";
		WiFiUDP udp;
		uint16_t port = 0;

		std::list<DNSEntry> entries;

		void replyWithIP(DNSHeader requestHeader, unsigned char *requestDomain, size_t length, IPAddress a);

};


#endif
