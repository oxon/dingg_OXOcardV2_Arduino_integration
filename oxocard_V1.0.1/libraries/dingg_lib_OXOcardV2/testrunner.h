#ifndef _LIB_TEST_TEST_RUNNER_H_
#define _LIB_TEST_TEST_RUNNER_H_
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
#include <list>
#include <stdint.h>
#include "testsuite.h"

namespace Test{
	class TestRunner{

		public:
			TestRunner(){};
			~TestRunner(){};

			int8_t run();
			int8_t addTestSuite(TestSuite *ts);

			static TestRunner *getDefaultSuites();
		protected:

		private:
			const char *LOG_TAG = "TestRunner";
			std::list<TestSuite *> testSuites;

	};
}
#endif
