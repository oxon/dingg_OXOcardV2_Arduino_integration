#include <esp_log.h>
#include "oxocard.h"
#include "pin_defines.h"
#include "accesspointscenario.h"
#include "firstbootscenario.h"
#include "flashscenario.h"
#include "pairingscenario.h"
#include "scenario.h"
#include "auto_turn_off.h"
#include "startup.h"
#include "system.h"

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* Uncomment to use the testing framework:
 	! The components.mk file need to change as well ! */
// #define ENABLE_TESTS 1

#ifdef ENABLE_TESTS
	#include "testrunner.h"
#endif

OXOcard oxocard;

TaskHandle_t userMainHandle = NULL;
TaskHandle_t autoTurnOffHandle = NULL;
TaskHandle_t manuallyTurnOffHandle = NULL;
TaskHandle_t lowBatteryHandle = NULL;
TaskHandle_t scenariosHandle = NULL;

extern void user_main();

static void user_main_task(void *pv){
	/* wait until no button is pushed before starting the user main */
	while (button1Pressed || button2Pressed || button3Pressed ||
		button4Pressed || button5Pressed || button6Pressed) {
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
	ESP_LOGI("Main", "Start the user code\n");
	user_main();
	/* If the users main function returns for some reason, turn off the OXOcard */
	oxocard.system->turnOff();
}

static void handle_auto_turn_off_task(void *pv){
	AutoTurnOff& ato = AutoTurnOff::getInstance();
	while(1){
		if (ato.isTriggered()) {
			xSemaphoreTake(oxocard.matrix->updateSemaphore, portMAX_DELAY);
			if (userMainHandle != NULL) vTaskSuspend(userMainHandle);
			if (manuallyTurnOffHandle != NULL) vTaskSuspend(manuallyTurnOffHandle);
			if (lowBatteryHandle != NULL) vTaskSuspend(lowBatteryHandle);
			if (scenariosHandle != NULL) vTaskSuspend(scenariosHandle);
			xSemaphoreGive(oxocard.matrix->updateSemaphore);
			oxocard.system->turnOff();
		}
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

static void handle_manually_turn_off_task(void *pv){
	while(1){
		if (button6Pressed && button5Pressed){
			if (button1Pressed){
				while(button6Pressed && button5Pressed){
					vTaskDelay(50/portTICK_PERIOD_MS);
				}
				if (button1Pressed) {
					xSemaphoreTake(oxocard.matrix->updateSemaphore, portMAX_DELAY);
					if (userMainHandle != NULL) vTaskSuspend(userMainHandle);
					if (autoTurnOffHandle != NULL) vTaskSuspend(autoTurnOffHandle);
					if (lowBatteryHandle != NULL) vTaskSuspend(lowBatteryHandle);
					if (scenariosHandle != NULL) vTaskSuspend(scenariosHandle);
					xSemaphoreGive(oxocard.matrix->updateSemaphore);
					oxocard.system->turnOff();
				}
			}
		}
		vTaskDelay(50/portTICK_PERIOD_MS);
	}
}

static void handle_low_battery_task(void *pv){
	/* get isCharging from the NVS */
	PersistentMemory pm = PersistentMemory("battery",
		PersistentMemory::WRITE_MODE);
	uint8_t isCharging = false;
	int8_t err = pm.getByte((const char*)("isCharging"), &isCharging);
	if (err < 0) {ESP_LOGE("Main", "Reading isCharging failed!");}
	/* in every case: reset isCharging in the NVS */
	if(pm.saveByte((const char*)("isCharging"), 0) < 0) {
		ESP_LOGE("Main", "Something is really wrong with saving isCharging.");
		return;
	}
	while(1) {
		oxocard.battery->handleLowBattery(isCharging,
			&userMainHandle, &autoTurnOffHandle, &scenariosHandle);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

static void scenarios_task(void *pv){
	System::wakeUpCause_t wakeUpCause = oxocard.system->getWakeUpCause();
	ESP_LOGI("Main", "wakeUpCause = %s", oxocard.system->getWakeUpCauseText());
	if(wakeUpCause >= System::WUC_AP_MODE_REQUEST &&
		wakeUpCause <= System::WUC_PAIRING_REQUEST) {
		xSemaphoreTake(oxocard.matrix->updateSemaphore, portMAX_DELAY);
		if (userMainHandle != NULL) vTaskSuspend(userMainHandle);
		xSemaphoreGive(oxocard.matrix->updateSemaphore);
		oxocard.audio->noTone();
		switch (uint8_t(wakeUpCause)) {
			case System::WUC_PAIRING_REQUEST: {
				PairingScenario p = PairingScenario(&oxocard);
				p.run();
			} break;
			case System::WUC_AP_MODE_REQUEST: {
				if (autoTurnOffHandle != NULL) vTaskSuspend(autoTurnOffHandle);
				AccessPointScenario p = AccessPointScenario(&oxocard);
				p.run();
				vTaskResume(autoTurnOffHandle);
			} break;
			case System::WUC_UPDATE_REQUEST: {
				FlashScenario p = FlashScenario(&oxocard);
				p.run();
			} break;
		}
		vTaskResume(userMainHandle);
	}
	scenariosHandle = NULL;
	vTaskDelete(scenariosHandle);	// deletes itself when NULL
}

extern "C" void app_main()
{
	/* Starting up */
	Startup s(&oxocard);
	s.run();

	#ifdef ENABLE_TESTS
		Test::TestRunner *r = Test::TestRunner::getDefaultSuites();
		r->run();
	#endif

	/* start relevant tasks (configMAX_PRIORITIES = 25) */
	xTaskCreate(&scenarios_task, "scenarios", 8192, NULL, 3, &scenariosHandle);
	xTaskCreate(&handle_low_battery_task, "low_battery", 2048, NULL, 3, &lowBatteryHandle);
	xTaskCreate(&handle_auto_turn_off_task, "auto_turn_off", 2048, NULL, 2, &autoTurnOffHandle);
	xTaskCreate(&handle_manually_turn_off_task, "manually_turn_off", 2048, NULL, 2, &manuallyTurnOffHandle);
	xTaskCreate(&user_main_task, "user_main", 16384, NULL, 1, &userMainHandle);
	while(1) vTaskDelay(portMAX_DELAY);
}
