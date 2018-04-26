#ifndef _LIB_TEST_TEST_CONTEXT_H_
#define _LIB_TEST_TEST_CONTEXT_H_
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
#include "testresult.h"

namespace Test{
	class TestContext; //TODO Circular dependency. NOGO, but no better solution as for now
	class TestContext{

		public:
			TestContext(void (*testMethod)(TestContext *c));
			~TestContext();

			TestResult *run();
			void setName(const char *name);
			void checkForMemoryLeaks();

			int8_t assertNULL(const char *desc, void *a);
			int8_t assertNotNULL(const char *desc, void *a);
			int8_t assertTerminated(const char *desc, const char *a);
			int8_t assertTerminated(const char *desc, const char *a, size_t maxLength);
			int8_t assertEmptyString(const char *desc, const char *a);

			int8_t assertEqual(const char *desc, const char *a, const char *b);
			int8_t assertEqual(const char *desc, int64_t a, int64_t b);
			int8_t assertEqual(const char *desc, int32_t a, int32_t b);
			int8_t assertEqual(const char *desc, int16_t a, int16_t b);
			int8_t assertEqual(const char *desc, int8_t a, int8_t b);
			int8_t assertEqual(const char *desc, uint64_t a, uint64_t b);
			int8_t assertEqual(const char *desc, uint32_t a, uint32_t b);
			int8_t assertEqual(const char *desc, uint16_t a, uint16_t b);
			int8_t assertEqual(const char *desc, uint8_t a, uint8_t b);

			int8_t assertBoolTrue(const char *desc, int8_t a);
			int8_t assertBoolFalse(const char *desc, int8_t a);

			int8_t markAsSuccess();
			int8_t markAsSuccess(const char *markMessage);
			int8_t markAsFailure();
			int8_t markAsFailure(const char *markMessage);

			int8_t success = 0;

		protected:

		private:
			const char *LOG_TAG = "TestContext";
			void (*testMethod)(TestContext *c) = NULL;

			TestResult *result = NULL;
			uint8_t isRunning = 0;

	};
}
#endif
