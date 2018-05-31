#ifndef _LIB_FIRSTBOOT_SCENARIO_H_
#define _LIB_FIRSTBOOT_SCENARIO_H_ 1
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
#include <stdint.h>
#include "images.h"
#include "scenario.h"
#include "accesspoint.h"
#include "dnsserver.h"

class FirstBootScenario : public Scenario{
	public:
		FirstBootScenario(OXOcard *card) : Scenario(card){};
	protected:
		int8_t configure(void);
		int8_t execute(void);
	private:
		const char *LOG_TAG = "FirstBootScenario";
};

#endif