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
#include <string.h>
#include "communication.h"
#include "testcommunication.h"

using namespace Test;

void testSenderField(TestContext *c){
	c->setName("testSenderField in LocalMessage");
	const char *rawMessage = "\x1d\x1foxocard\x1d\x1f{\"s\":\"asender\", \"r\":\"recv\", \"um\":\"message\", \"un\": 23}";
	Communication::LocalMessage *lm = new Communication::LocalMessage(rawMessage);
	c->assertNotNULL("sender != NULL", (void *)lm->sender);
	c->assertTerminated("sender is terminated", lm->sender);
	c->assertEqual("sender is equal to input", "asender", lm->sender);
	delete lm;

	const char *rawMessage2 = "\x1d\x1foxocard\x1d\x1f{\"s\":\"\", \"r\":\"recv\", \"um\":\"message\", \"un\": 23}";
	lm = new Communication::LocalMessage(rawMessage2);
	c->assertNotNULL("sender != NULL", (void *)lm->sender);
	c->assertTerminated("sender is terminated", lm->sender);
	c->assertEmptyString("sender is empty", lm->sender);
	delete lm;

	const char *rawMessage3 = "\x1d\x1foxocard\x1d\x1f{\"r\":\"recv\", \"um\":\"message\", \"un\": 23}";
	lm = new Communication::LocalMessage(rawMessage3);
	c->assertNULL("sender == NULL", (void *)lm->sender);
	delete lm;
}

void testReceiverField(TestContext *c){
	c->setName("testReceiverField in LocalMessage");
	const char *rawMessage = "\x1d\x1foxocard\x1d\x1f{\"s\":\"s\", \"r\":\"arecv\", \"um\":\"message\", \"un\": 23}";
	Communication::LocalMessage *lm = new Communication::LocalMessage(rawMessage);
	c->assertNotNULL("receiver != NULL", (void *)lm->receiver);
	c->assertTerminated("receiver is terminated", lm->receiver);
	c->assertEqual("receiver is equal to input", "arecv", lm->receiver);
	delete lm;

	const char *rawMessage2 = "\x1d\x1foxocard\x1d\x1f{\"s\":\"s\", \"r\":\"\", \"um\":\"message\", \"un\": 23}";
	lm = new Communication::LocalMessage(rawMessage2);
	c->assertNotNULL("receiver != NULL", (void *)lm->receiver);
	c->assertTerminated("receiver is terminated", lm->receiver);
	c->assertEmptyString("receiver is empty", lm->receiver);
	delete lm;

	const char *rawMessage3 = "\x1d\x1foxocard\x1d\x1f{\"s\":\"s\", \"um\":\"message\", \"un\": 23}";
	lm = new Communication::LocalMessage(rawMessage3);
	c->assertNULL("receiver == NULL", (void *)lm->receiver);
	delete lm;
}

void testMessageField(TestContext *c){
	c->setName("testMessageField in LocalMessage");
	const char *rawMessage = "\x1d\x1foxocard\x1d\x1f{\"s\":\"s\", \"r\":\"arecv\", \"um\":\"themessage\", \"un\": 23}";
	Communication::LocalMessage *lm = new Communication::LocalMessage(rawMessage);
	c->assertNotNULL("message != NULL", (void *)lm->userMessage);
	c->assertTerminated("message is terminated", lm->userMessage);
	c->assertEqual("message is equal to input", "themessage", lm->userMessage);
	delete lm;

	const char *rawMessage2 = "\x1d\x1foxocard\x1d\x1f{\"s\":\"s\", \"r\":\"\", \"um\":\"\", \"un\": 23}";
	lm = new Communication::LocalMessage(rawMessage2);
	c->assertNotNULL("message != NULL", (void *)lm->userMessage);
	c->assertTerminated("message is terminated", lm->userMessage);
	c->assertEmptyString("message is empty", lm->userMessage);
	delete lm;

	const char *rawMessage3 = "\x1d\x1foxocard\x1d\x1f{\"s\":\"s\", r:\"recv\", \"un\": 23}";
	lm = new Communication::LocalMessage(rawMessage3);
	c->assertNULL("message == NULL", (void *)lm->userMessage);
	delete lm;
}

void testPackAndUnpack(TestContext *c){
	c->setName("packing and unpacking LocalMessage");

	const char *sender = "from";
	const char *message = "howdy howdy";
	const char *receiver = "tototo";
	uint32_t number = 42;
	Communication::LocalMessage *lm = new Communication::LocalMessage(sender, message, receiver, number);
	Communication::LocalMessage *lm2 = new Communication::LocalMessage(lm->message);

	c->assertNotNULL("sender != NULL", (void *)lm2->sender);
	c->assertNotNULL("receiver != NULL", (void *)lm2->receiver);
	c->assertNotNULL("message != NULL", (void *)lm2->userMessage);

	c->assertTerminated("sender is terminated", lm2->sender);
	c->assertTerminated("receiver is terminated", lm2->receiver);
	c->assertTerminated("message is terminated", lm2->userMessage);

	c->assertEqual("sender is equal to input", sender, lm2->sender);
	c->assertEqual("receiver is equal to input", receiver, lm2->receiver);
	c->assertEqual("message is equal to input", message, lm2->userMessage);

	c->assertEqual("number is equal to input", number, lm2->userNumber);

	delete lm; delete lm2;
}

void checkForMemoryLeak(TestContext *c){
	c->setName("memory leak test");
	c->checkForMemoryLeaks();
	for(int i=0; i<5000; i++){
		Communication::LocalMessage *lm = new Communication::LocalMessage("from", "howdy howdy", "tototo", 23);
		Communication::LocalMessage *lm2 = new Communication::LocalMessage(lm->message);
		delete lm; delete lm2;
	}
	c->assertTerminated("dummytest", "asdf");
}

TestSuiteCommunication::TestSuiteCommunication(){
	this->name = "Communication";
	this->tests.push_back(&testSenderField);
	this->tests.push_back(&testReceiverField);
	this->tests.push_back(&testMessageField);
	this->tests.push_back(&testPackAndUnpack);
	this->tests.push_back(&checkForMemoryLeak);
}
