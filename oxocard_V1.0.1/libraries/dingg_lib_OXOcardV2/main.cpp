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

#ifdef FB_LOG_OUTPUT
extern "C" {
#include "udp_logging.h"
}
#endif

OXOcard oxocard;

volatile SemaphoreHandle_t startMainTaskSemaphore;
volatile SemaphoreHandle_t startScenarioTaskSemaphore;

TaskHandle_t userMainHandle = NULL;
TaskHandle_t autoTurnOffHandle = NULL;
TaskHandle_t manuallyTurnOffHandle = NULL;
TaskHandle_t lowBatteryHandle = NULL;
TaskHandle_t scenariosHandle = NULL;

PairingScenario *pairingScenarioPtr = NULL;
AccessPointScenario *apScenarioPtr = NULL;
FlashScenario *flashScenarioPtr = NULL;

extern void user_main();
static void deleteAllTasksExcept(TaskHandle_t exceptedTask);

static void user_main_task(void *pv){
	xSemaphoreGive(startMainTaskSemaphore);	// main task started
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
			deleteAllTasksExcept(autoTurnOffHandle);
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
					deleteAllTasksExcept(manuallyTurnOffHandle);
					oxocard.system->turnOff();
				}
			}
		}
		vTaskDelay(50/portTICK_PERIOD_MS);
	}
}

static void handle_low_battery_task(void *pv){
	while(1) {
		if (oxocard.battery->isLowBattery()) {
			/* wait until the scenario task has been created before suspending */
			xSemaphoreTake(startScenarioTaskSemaphore, portMAX_DELAY);
			deleteAllTasksExcept(lowBatteryHandle);
			oxocard.battery->handleLowBattery(oxocard.matrix, oxocard.system);
		}
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

static void scenarios_task(void *pv){
	xSemaphoreGive(startScenarioTaskSemaphore);	// scenario task started
	System::wakeUpCause_t wakeUpCause = oxocard.system->getWakeUpCause();
	ESP_LOGI("Main", "wakeUpCause = %s", oxocard.system->getWakeUpCauseText());
	if(wakeUpCause >= System::WUC_AP_MODE_REQUEST &&
		wakeUpCause <= System::WUC_PAIRING_REQUEST) {
		oxocard.audio->noTone();
		switch (uint8_t(wakeUpCause)) {
			case System::WUC_PAIRING_REQUEST: {
				if (autoTurnOffHandle != NULL) vTaskSuspend(autoTurnOffHandle);
				pairingScenarioPtr = new PairingScenario(&oxocard);
				pairingScenarioPtr->run();
				delete &pairingScenarioPtr;
				vTaskResume(autoTurnOffHandle);
			} break;
			case System::WUC_AP_MODE_REQUEST: {
				if (autoTurnOffHandle != NULL) vTaskSuspend(autoTurnOffHandle);
				apScenarioPtr = new AccessPointScenario(&oxocard);
				apScenarioPtr->run();
				delete &apScenarioPtr;
				vTaskResume(autoTurnOffHandle);
			} break;
			case System::WUC_UPDATE_REQUEST: {
				flashScenarioPtr = new FlashScenario(&oxocard);
				flashScenarioPtr->run();
				delete &flashScenarioPtr;
			} break;
		}
	}
	xSemaphoreGive(startMainTaskSemaphore);
	scenariosHandle = NULL;
	vTaskDelete(scenariosHandle);	// deletes itself when NULL
}

void deleteAllTasksExcept(TaskHandle_t exceptedTaskHandle){
	xSemaphoreTake(oxocard.matrix->updateSemaphore, portMAX_DELAY);
	if (exceptedTaskHandle != lowBatteryHandle && lowBatteryHandle != NULL) vTaskDelete(lowBatteryHandle);
	if (exceptedTaskHandle != autoTurnOffHandle && autoTurnOffHandle != NULL) vTaskDelete(autoTurnOffHandle);
	if (exceptedTaskHandle != manuallyTurnOffHandle && manuallyTurnOffHandle != NULL) vTaskDelete(manuallyTurnOffHandle);
	if (userMainHandle != NULL) vTaskDelete(userMainHandle);
	if (scenariosHandle != NULL) vTaskDelete(scenariosHandle);
	if (pairingScenarioPtr != NULL && pairingScenarioPtr->animationsHandle != NULL) vTaskDelete(pairingScenarioPtr->animationsHandle);
	if (apScenarioPtr != NULL && apScenarioPtr->animationsHandle != NULL) vTaskDelete(apScenarioPtr->animationsHandle);
	xSemaphoreGive(oxocard.matrix->updateSemaphore);
}

extern "C" void app_main()
{
	/* Starting up */
	Startup s(&oxocard);
	s.run();

	#ifdef FB_LOG_OUTPUT
		oxocard.wifi->autoConnectWithFeedback(oxocard.matrix);
		udp_logging_init( "192.168.42.255", 1337 );
	#endif

	#ifdef ENABLE_TESTS
		Test::TestRunner *r = Test::TestRunner::getDefaultSuites();
		r->run();
	#endif

	/* start relevant tasks (configMAX_PRIORITIES = 25) */
	startMainTaskSemaphore = xSemaphoreCreateBinary();			// creates a taken semaphore
	startScenarioTaskSemaphore = xSemaphoreCreateBinary();	// creates a taken semaphore
	xTaskCreate(&handle_auto_turn_off_task, "auto_turn_off", 2048, NULL, 3, &autoTurnOffHandle);
	xTaskCreate(&handle_manually_turn_off_task, "manually_turn_off", 2048, NULL, 3, &manuallyTurnOffHandle);
	xTaskCreate(&handle_low_battery_task, "low_battery", 2048, NULL, 3, &lowBatteryHandle);
	xTaskCreate(&scenarios_task, "scenarios", 8192, NULL, 2, &scenariosHandle);
	/* wait until the scenario task is finished before starting the main task */
	xSemaphoreTake(startMainTaskSemaphore, portMAX_DELAY);
	xTaskCreate(&user_main_task, "user_main", 16384, NULL, 1, &userMainHandle);
	while(1) vTaskDelay(portMAX_DELAY);
}
