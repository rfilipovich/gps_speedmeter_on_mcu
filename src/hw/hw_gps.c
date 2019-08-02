#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/projdefs.h"

#include "esp_log.h"
#include "driver/uart.h"
#include "minmea.h"

static char tag[] = "gps";

/* only for debug on table */
//#define DBG_GENERATE_GPS_DATA


#define GPS_RX_PIN (34)
#define GPS_UART_NUM (UART_NUM_1)

volatile uint32_t ext_speed = 0;
volatile bool gps_valid = false;

static QueueHandle_t uart_queue = NULL;


/////////////////////////////////////////////////////
//checking time for 1 houndred km/h

#define MAX_TRIGGERS (4)

volatile uint32_t trigger_0_ms = 0;
volatile uint32_t trigger_1_ms = 0;
volatile uint32_t trigger_2_ms = 0;
volatile uint32_t trigger_3_ms = 0;

static struct _trig_lim_str_t {
	TickType_t  sys_ticks;
	uint32_t    trigged_ms;
	int         status;
} _trig_lim_str[MAX_TRIGGERS] = {
#if MAX_TRIGGERS >= 1
		{0, 0, 0},
#endif
#if MAX_TRIGGERS >= 2
		{0, 0, 0},
#endif
#if MAX_TRIGGERS >= 3
		{0, 0, 0},
#endif
#if MAX_TRIGGERS >= 4
		{0, 0, 0},
#endif
};


static void _trig_one_limit(
		struct _trig_lim_str_t *const str,
		const uint32_t current_speed,
		const uint16_t trig_speed,
		const bool __clear) {

	if(__clear || (0 == current_speed)) {
		str->sys_ticks = 0;
		str->status = 0;

		if(__clear) {
			str->trigged_ms = 0;
			return;
		}
	};

	switch(str->status) {
		case 0: {
			if(current_speed == 0) {
				str->status = 1;
			}
			break;
		};

		case 1: {
			if(current_speed > 0) {
				str->trigged_ms = 0; /* clear last speed */
				str->sys_ticks = xTaskGetTickCount();
				str->status = 2;
			}
			break;
		};

		case 2: {
			if(current_speed >= trig_speed) {
				str->trigged_ms = (xTaskGetTickCount() - str->sys_ticks) * portTICK_PERIOD_MS;
				str->status = 0;
			};
			break;
		};

		default: {
			str->status = 0;
			break;
		};
	}
}

static void _check_and_apply_trigers(const uint32_t current_speed_mul_thousen, const bool __clear)
{
	const uint32_t speed_km_h = (current_speed_mul_thousen / 1000);

	/* for 40 km/h */
	_trig_one_limit(&_trig_lim_str[0], speed_km_h, 40, __clear);
	trigger_0_ms = _trig_lim_str[0].trigged_ms;

	/* for 60 km/h */
	_trig_one_limit(&_trig_lim_str[1], speed_km_h, 60, __clear);
	trigger_1_ms = _trig_lim_str[1].trigged_ms;

	/* for 80 km/h */
	_trig_one_limit(&_trig_lim_str[2], speed_km_h, 80, __clear);
	trigger_2_ms = _trig_lim_str[2].trigged_ms;

	/* for 100 km/h */
	_trig_one_limit(&_trig_lim_str[3], speed_km_h, 100, __clear);
	trigger_3_ms = _trig_lim_str[3].trigged_ms;
}

/////////////////////////////////////////////////////


static char *readLine(uart_port_t uart, const size_t g_size) {
	static char line[256];
	int size;
	char *ptr = line;
    char *const ptrEnd = ptr + sizeof(line)/sizeof(*line);

    size = uart_read_bytes(uart, (unsigned char *)ptr, (g_size%256), portMAX_DELAY);

	while(ptr < ptrEnd) {
		//!size = uart_read_bytes(uart, (unsigned char *)ptr, 1, portMAX_DELAY);
	//	if (size == 1) {
			if (*ptr == '\n') {
				ptr++;
				*ptr = '\0';
				return line;
			}
			ptr++;
//		}
	} 

	return "\0";
} 


static void task_gps(void *ignore)   {
	ESP_LOGD(tag, ">> doGPS");

	uart_event_t event;

	uart_config_t myUartConfig;
	myUartConfig.baud_rate           = 115200;
	myUartConfig.data_bits           = UART_DATA_8_BITS;
	myUartConfig.parity              = UART_PARITY_DISABLE;
	myUartConfig.stop_bits           = UART_STOP_BITS_1;

	myUartConfig.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE;
	myUartConfig.rx_flow_ctrl_thresh = 120;

	uart_param_config(GPS_UART_NUM, &myUartConfig);

	uart_set_pin(GPS_UART_NUM,
			UART_PIN_NO_CHANGE, // TX
			GPS_RX_PIN,         // RX
			UART_PIN_NO_CHANGE, // RTS
			UART_PIN_NO_CHANGE  // CTS
    );

	uart_driver_install(GPS_UART_NUM, 2048, 2048, 120, &uart_queue, 0);

    //Waiting for UART event.
    for(;;) {

//!    	vTaskDelay(100 / portTICK_PERIOD_MS);
    	//event.type = UART_DATA;
		if (xQueueReceive(uart_queue, (void * )&event, (portTickType) portMAX_DELAY)) {

//!			ESP_LOGD(tag, "UART Event %d ##\n", event.type );

			switch (event.type)
			{
				//Event of UART receving data
				case UART_DATA:
					if (event.size) {
						char *line = readLine(GPS_UART_NUM, event.size);


						//ESP_LOGD(tag, "%s", line);
						switch(minmea_sentence_id(line, false)) {
							case MINMEA_SENTENCE_RMC: {
								  //ESP_LOGD(tag, "Sentence - MINMEA_SENTENCE_RMC");
								  struct minmea_sentence_rmc frame;

								  if (minmea_parse_rmc(&frame, line)) {
									  gps_valid = frame.valid;
#if !defined(DBG_GENERATE_GPS_DATA)
									  if(frame.valid) {
										  ext_speed = minmea_rescale(&frame.speed, 1000);
	
										  /* convert from knots to km/h */
										  ext_speed *= 1852;
										  ext_speed /= 1000;
									  } else {
										  ext_speed = 0;
									  };
#else
									  ///DBG///
									  ext_speed += 10*1000;
									  if(ext_speed > 150*1000) {
										  ext_speed = 0;
									  }

									  gps_valid = true;
#endif
									  _check_and_apply_trigers(ext_speed, !gps_valid);

								  };/* else {
									  ESP_LOGD(tag, "$xxRMC sentence is not parsed\n");
								  }*/
								break;
							};
							case MINMEA_SENTENCE_GGA:
								//ESP_LOGD(tag, "Sentence - MINMEA_SENTENCE_GGA");
								break;
							case MINMEA_SENTENCE_GSV:
								//ESP_LOGD(tag, "Sentence - MINMEA_SENTENCE_GSV");
								break;
							default:
								//ESP_LOGD(tag, "Sentence - other");
								break;
							}
					}
					break;
				default:
					break;
			}
		}
	}

	vTaskDelete(NULL);
};

int32_t gps_init(void)
{
	xTaskCreate(&task_gps, "gps", 8048, NULL, 100, NULL);

	return 0;
}

