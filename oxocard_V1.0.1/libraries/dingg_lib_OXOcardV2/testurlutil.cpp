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
#include "testurlutil.h"
#include "urlutil.h"

using namespace Test;

void testEncodeURL(TestContext *c){
	c->setName("encoding urls");
	const char *source = "asdf8(&/isdfl asdf";
	const char *encoded = URLUtil::encode(source);
	c->assertNotNULL("encoded is not null.", (void *)encoded);
	c->assertTerminated("encoded is properly terminated.", encoded);
	c->assertEqual("is properly encoded", encoded, "asdf8%28%26%2Fisdfl%20asdf");
}

TestSuiteURLUtil::TestSuiteURLUtil(){
	this->name = "URLUtil";
	this->tests.push_back(&testEncodeURL);
}
