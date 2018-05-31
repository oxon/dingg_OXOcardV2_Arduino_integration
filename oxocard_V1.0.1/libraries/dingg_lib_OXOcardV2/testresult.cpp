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
#include "testresult.h"

using namespace Test;

AssertResult::AssertResult(const char *desc, int8_t success){
	this->desc = desc;
	this->success = success;
}

int8_t TestResult::registerAssert(const char *desc, int8_t success){
	size_t rounded = (size_t)((strlen(desc)+1)/4)*4;
	this->usedMemory += rounded+4;
	this->asserts.push_back(new AssertResult(desc, success));
	return 0;
}

int8_t TestResult::assertSuccess(){
	for(auto assert = this->asserts.begin(); assert != this->asserts.end(); ++assert) {
		if((*assert)->success !=1 )
			return -1;
	}
	return 1;
}

size_t TestResult::getAssertionCount(){
	return this->asserts.size();
}

size_t TestResult::getAssertionFailureCount(){
	size_t count = 0;
	for(auto assert = this->asserts.begin(); assert != this->asserts.end(); ++assert) {
		if((*assert)->success !=1 )
			count++;
	}
	return count;
}

void TestResult::printAssertionFailures(){
	for(auto assert = this->asserts.begin(); assert != this->asserts.end(); ++assert) {
		if((*assert)->success !=1 )
			ESP_LOGE(this->LOG_TAG, "Assertion \"%s\" failed!", (*assert)->desc);
	}
}
