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
#include "testresult.h"
#include "testsuite.h"

using namespace Test;

int8_t TestSuite::run(){
	ESP_LOGI(this->LOG_TAG, "===== Running TestSuite \"%s\" =====", this->name);
	for(auto test = this->tests.begin(); test != this->tests.end(); ++test) {
		TestContext *tc = new TestContext(*test);
		TestResult *result = tc->run();
		delete tc;
		this->printResults(result);
		delete result;

	}
	return 0;
}


void TestSuite::printResults(TestResult *result){
	ESP_LOGI(this->LOG_TAG, "Test \"%s\"  took %d ms.", result->name, (result->endTime-result->startTime));
	if(result->getAssertionCount() > 0 && result->assertSuccess() == 1){
		ESP_LOGI(this->LOG_TAG, "All %d assertions we're OK.", result->getAssertionCount());
	}else{
		result->printAssertionFailures();
	}


	if(result->failMemoryLeaks == 0) return;

	int32_t explained = result->getAssertionCount() * sizeof(AssertResult);
	//explained += sizeof(TestResult);
	ESP_LOGI(this->LOG_TAG, "We may can explain heap: %d", explained);
	ESP_LOGI(this->LOG_TAG, "used mem: %d", result->usedMemory);
	int32_t diff = (int32_t)result->startHeap - (int32_t)result->endHeap;
	ESP_LOGI(this->LOG_TAG, "Heap at start %d and end %d. Difference: %d", result->startHeap, result->endHeap, diff);
	if(diff > 0){
		ESP_LOGE(this->LOG_TAG, "We *MAY* have a memory leak of %d bytes", diff);
	}

}
