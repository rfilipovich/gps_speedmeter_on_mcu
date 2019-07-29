#include <stdint.h>

#include "esp_log.h"
#include "hw_board.h"
#include "hw_uart.h"
#include "hw_display.h"
#include "hw_gps.h"


static const char* MESSAGE_TAG = "BRD";


uint32_t hw_board_init(void)
{
	int ret = -1;

	/* initialization Debug uart */
	ESP_LOGI(MESSAGE_TAG, "* Debug UART");

//!!!    uart_debug_init();


/* initialize OLED and start task */
	ESP_LOGI(MESSAGE_TAG, "* Display");

	if(0 > (ret = display_init())) {
		return ret;
	};

/* initialize GPS and start task */
	if(0 > (ret = gps_init())) {
		return ret;
	};

	return 0;
}

