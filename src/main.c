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

static const char* MAIN_TAG = "MAIN";
#define MAIN_NODE_LOGI( format, ... ) ESP_LOGI( MAIN_TAG, format, ##__VA_ARGS__ )
#define MAIN_NODE_LOGE( format, ... ) ESP_LOGE( MAIN_TAG, format, ##__VA_ARGS__ )
#define MAIN_NODE_LOGW( format, ... ) ESP_LOGW( MAIN_TAG, format, ##__VA_ARGS__ )

void app_main()
{
	esp_log_level_set("gpio", ESP_LOG_ERROR);

	MAIN_NODE_LOGI( "Initialising...");

	hw_board_init();

	MAIN_NODE_LOGI( "------------------------------------------");

	// Version info
	MAIN_NODE_LOGI( "Version IDF: %s", esp_get_idf_version());

	// Print parameters for device
	vTaskDelay(100 / portTICK_PERIOD_MS);

	MAIN_NODE_LOGI( "------------------------------------------");

	MAIN_NODE_LOGI( "Show start logo...");
//!	hb_display_show_screen(DEMO_START);
	vTaskDelay(1500 / portTICK_PERIOD_MS);

	/////////// start nodes ///////////////////////////////////

	MAIN_NODE_LOGI( "* Start nodes");

//	hb_node_start_processing();

	MAIN_NODE_LOGI( "Initialising completed!");

	// Infifinity loop
//	for(;;) {
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//	}
}
