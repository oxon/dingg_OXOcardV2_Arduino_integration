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
#include "testrunner.h"
#include "testurlutil.h"
#include "testcommunication.h"

using namespace Test;

int8_t TestRunner::run(){
	ESP_LOGI(this->LOG_TAG, "Tests running... ");
	for(auto testSuite = this->testSuites.begin(); testSuite != this->testSuites.end(); ++testSuite) {
		(*testSuite)->run();
	}

	return 0;
}

int8_t TestRunner::addTestSuite(TestSuite *ts){
	this->testSuites.push_back(ts);
	return 0;
}

TestRunner *TestRunner::getDefaultSuites(){
	TestRunner *r = new TestRunner();
	r->addTestSuite(new TestSuiteCommunication());
	r->addTestSuite(new TestSuiteURLUtil());
	return r;
}
