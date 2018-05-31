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
#include "accesspointscenario.h"
#include "firstbootscenario.h"
#include "oxocard.h"

int8_t FirstBootScenario::execute(void){
	ESP_LOGI(this->LOG_TAG, "Running firstboot.");
	this->oxocard->matrix->setForeColor(rgb(0,0,255));
	this->oxocard->matrix->drawText(System::getName(), true, false);
	this->oxocard->matrix->drawText(System::getName(), true, false);
	AccessPointScenario p = AccessPointScenario(this->oxocard);
	p.run();
	return 0;
}

int8_t FirstBootScenario::configure(void){
	ESP_LOGI(this->LOG_TAG, "Configure firstboot.");

	this->oxocard->matrix->clear();

	return 0;
}
