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

#include <esp_heap_caps.h>
#include <esp_log.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include "testcontext.h"

using namespace Test;

TestContext::TestContext(void (*testMethod)(TestContext *c)){
	this->testMethod = testMethod;
	this->result = new TestResult();
}

TestContext::~TestContext(){
}

void TestContext::setName(const char *name){
	this->result->name = name;
}

TestResult *TestContext::run(){
	this->result->startHeap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
	this->isRunning = 1;
	this->result->startTime =  (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC);
	(this->testMethod)(this);
	this->result->endTime = (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC);
	this->isRunning = 0;
	this->result->endHeap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
	return this->result;
}

void TestContext::checkForMemoryLeaks(){
	this->result->failMemoryLeaks = 1;
}

int8_t TestContext::markAsSuccess(){
	this->result->markSuccess = 1;
	return 0;
}

int8_t TestContext::markAsSuccess(const char *markMessage){
	this->result->markSuccess = 1;
	this->result->markMessage = markMessage;
	return 0;
}

int8_t TestContext::markAsFailure(){
	this->result->markSuccess = -1;
	return 0;
}

int8_t TestContext::markAsFailure(const char *markMessage){
	this->result->markSuccess = -1;
	this->result->markMessage = markMessage;
	return 0;
}

int8_t TestContext::assertEqual(const char *desc, uint64_t a, uint64_t b){
	int8_t result = (a==b) ? 1 : -1;
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertEqual(const char *desc, uint32_t a, uint32_t b){
	return this->assertEqual(desc, (uint64_t) a, (uint64_t) b);
}

int8_t TestContext::assertEqual(const char *desc, uint16_t a, uint16_t b){
	return this->assertEqual(desc, (uint64_t) a, (uint64_t) b);
}

int8_t TestContext::assertEqual(const char *desc, uint8_t a, uint8_t b){
	return this->assertEqual(desc, (uint64_t) a, (uint64_t) b);
}

int8_t TestContext::assertEqual(const char *desc, int64_t a, int64_t b){
	int8_t result = (a==b) ? 1 : -1;
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertEqual(const char *desc, int32_t a, int32_t b){
	return this->assertEqual(desc, (int64_t) a, (int64_t) b);
}

int8_t TestContext::assertEqual(const char *desc, int16_t a, int16_t b){
	return this->assertEqual(desc, (int64_t) a, (int64_t) b);
}

int8_t TestContext::assertEqual(const char *desc, int8_t a, int8_t b){
	return this->assertEqual(desc, (int64_t) a, (int64_t) b);
}

int8_t TestContext::assertEmptyString(const char *desc, const char *a){
	int8_t result = (strlen(a) == 0) ? 1 : -1;
	//ESP_LOGI("assertEmptyString", ">%s< (%d)", a, strlen(a));
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertEqual(const char *desc, const char *a, const char *b){
	int8_t result = (strcmp(a,b) == 0) ? 1 : 0;
	result = (strlen(a) == strlen(b)) ? result : 0;
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertTerminated(const char *desc, const char *a){
	return this->assertTerminated(desc, a, 128);
}

int8_t TestContext::assertTerminated(const char *desc, const char *a, size_t maxLength){
	size_t i=0;
	for(i=0; i<maxLength; i++){
		if(a[i] == '\0') break;
	}
	int8_t result = -1;
	if(a[i] == '\0') result = 1;
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertNULL(const char *desc, void *a){
	if(!this->isRunning) return -1;
	int8_t result = (a==NULL) ? 1 : -1;
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertNotNULL(const char *desc, void *a){
	if(!this->isRunning) return -1;
	int8_t result = (a!=NULL) ? 1 : -1;
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertBoolTrue(const char *desc, int8_t a){
	if(!this->isRunning) return -1;
	int8_t result = (a==1) ? 1 : -1;
	this->result->registerAssert(desc, result);
	return result;
}

int8_t TestContext::assertBoolFalse(const char *desc, int8_t a){
	if(!this->isRunning) return -1;
	int8_t result = (a==0) ? 1 : -1;
	this->result->registerAssert(desc, result);
	return result;
}
