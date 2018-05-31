#ifndef _LIB_TEST_TEST_RESULT_H_
#define _LIB_TEST_TEST_RESULT_H_
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

namespace Test{

	class AssertResult{
		public:
			AssertResult(const char*desc, int8_t success);
			const char* desc = "";
			int8_t success = 0;
	};

	class TestResult{

		public:
			TestResult(){};
			~TestResult(){};

			const char *name = "";
			int8_t failMemoryLeaks = 0;

			int8_t markSuccess = 0;
			const char *markMessage = "";

			uint32_t startTime = 0;
			uint32_t endTime = 0;
			size_t startHeap = 0;
			size_t endHeap = 0;
			size_t usedMemory = 0;
			int8_t registerAssert(const char *desc, int8_t success);
			int8_t assertSuccess();
			size_t getAssertionCount();
			size_t getAssertionFailureCount();
			void printAssertionFailures();
		protected:

		private:
			const char *LOG_TAG = "TestResult";
			std::list<AssertResult *> asserts;

	};
}
#endif
