#include "main.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"

#include "string.h"

#include "driver/uart.h"
//#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "debug.h"

#include "hw/hw_board.h"
#include "ArduinoNvs.h"

static const char* MAIN_TAG = "MAIN";
#define MAIN_NODE_LOGI( format, ... ) ESP_LOGI( MAIN_TAG, format, ##__VA_ARGS__ )
#define MAIN_NODE_LOGE( format, ... ) ESP_LOGE( MAIN_TAG, format, ##__VA_ARGS__ )
#define MAIN_NODE_LOGW( format, ... ) ESP_LOGW( MAIN_TAG, format, ##__VA_ARGS__ )

void app_main(void)
{
	esp_log_level_set("gpio", ESP_LOG_ERROR);

	MAIN_NODE_LOGI( "Initialising...");

	/* init nvs storage */
	NVS.begin();

	/* init all preifery on board */
	hw_board_init();

	MAIN_NODE_LOGI( "Show start logo...");


	/////////// start nodes ///////////////////////////////////
	MAIN_NODE_LOGI( "Initialising completed!");
}
