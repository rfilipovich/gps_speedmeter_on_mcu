#include <u8g2.h>
#include <stdint.h>

#include "hw_display.h"
#include "u8g2_esp32_hal.h"
#include "esp_log.h"

static const char* TAG = "LCD";

static u8g2_t u8g2; // a structure which will contain all the data for one display


// SDA - GPIO21
#define PIN_SDA 21

// SCL - GPIO22
#define PIN_SCL 22


extern volatile uint32_t ext_speed;
extern volatile bool gps_valid;

extern volatile uint32_t trigger_0_ms;
extern volatile uint32_t trigger_1_ms;
extern volatile uint32_t trigger_2_ms;
extern volatile uint32_t trigger_3_ms;

#define MAX_DISP_TRIGGERS (4)

struct _speed_triggers_t {
	bool is_one_hundred;
	uint32_t ms;
};

static struct _speed_triggers_t _speed_triggers[MAX_DISP_TRIGGERS] = {
#if MAX_DISP_TRIGGERS >= 1
		{false, 0},
#endif
#if MAX_DISP_TRIGGERS >= 2
		{false, 0},
#endif
#if MAX_DISP_TRIGGERS >= 3
		{false, 0},
#endif
#if MAX_DISP_TRIGGERS >= 4
		{false, 0},
#endif
};

#define _speed_triggers_cnt \
	(sizeof(_speed_triggers)/sizeof(_speed_triggers[0]))

static void _draw_speed_trigger(
		const uint16_t start_x,
		const uint16_t start_y,
		const uint32_t ms) {

	const uint32_t out_ms = ms/10;
	char speed_str[16] = "__.__";
	
	if(ms != 0) {
		if(out_ms > 9999) {
			sprintf(speed_str, "99.99");
		} else {
			sprintf(speed_str, "%02d.%02d",
				(out_ms/100)%100, out_ms%100);
		};
	};

	u8g2_SetFont(&u8g2, u8g2_font_helvB08_tf);
	u8g2_DrawStr(&u8g2, start_x, start_y, speed_str);
}

static void _draw_speed_tirggers_list(
		const uint16_t start_x,
		uint16_t start_y,
		const struct _speed_triggers_t *const list,
		const size_t list_cnt) {

	for(int i = 0; i < list_cnt; i++) {
		_draw_speed_trigger(start_x, start_y, list[i].ms);
		start_y += 10;
	};
}

static void _refresh_tirgger_list() {
	_speed_triggers[0].ms = trigger_0_ms;
	_speed_triggers[1].ms = trigger_1_ms;
	_speed_triggers[2].ms = trigger_2_ms;
	_speed_triggers[3].ms = trigger_3_ms;
}

void task_lcd(void *ignore) {

	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda   = PIN_SDA;
	u8g2_esp32_hal.scl  = PIN_SCL;
	u8g2_esp32_hal_init(u8g2_esp32_hal);

	u8g2_Setup_sh1106_i2c_128x64_winstar_f(
		&u8g2,
		U8G2_R0,
		u8g2_esp32_i2c_byte_cb, /* u8x8_byte_sw_i2c */
		u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure

	u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

	ESP_LOGI(TAG, "u8g2_InitDisplay");
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

	ESP_LOGI(TAG, "u8g2_SetPowerSave");
	u8g2_SetPowerSave(&u8g2, 0); // wake up display

	ESP_LOGI(TAG, "u8g2_ClearBuffer");
	u8g2_ClearBuffer(&u8g2);

	ESP_LOGI(TAG, "u8g2_SetFont");
	u8g2_SetFont(&u8g2, /*u8g2_font_ncenB24_tr*/u8g2_font_fub35_tf);

	ESP_LOGI(TAG, "u8g2_DrawStr");

	u8g2_UpdateDisplay(&u8g2);

	ESP_LOGI(TAG, "* Display");

#if 0
//	u8g2_setFontRefHeightAll(&u8g2);  	/* this will add some extra space for the text inside the buttons */
//	u8g2_userInterfaceMessage(&u8g2, "Title1", "Title2", "Title3", " Ok \n Cancel ");
//	u8g2_DrawCircle(&u8g2, 64, 32, 5, U8G2_DRAW_ALL);
//	u8g2_UpdateDisplay(&u8g2);
#endif

//	bool _blink_activity = false;
	for(;;) {
		u8g2_ClearBuffer(&u8g2);

		_refresh_tirgger_list();

		{/* draw speed */
			char speed_str[16] = {0};
			if(gps_valid) {
				sprintf(speed_str, "%03d",
						(ext_speed/1000)%1000
						/*, (ext_speed/100)%10*/);
			} else {
				sprintf(speed_str, "xxx");
			};
			
			u8g2_SetFont(&u8g2, u8g2_font_fub35_tf);
			u8g2_DrawStr(&u8g2, 2, 40, speed_str);
		};

#if 0	
		if(gps_valid) {
			if(_blink_activity) {
				u8g2_DrawDisc(&u8g2, 100, 10, 8, U8G2_DRAW_ALL);
			} else {
				u8g2_DrawCircle(&u8g2, 100, 10, 8, U8G2_DRAW_ALL);
			};

			_blink_activity ^= 1;
		} else {
			_blink_activity = false;
			u8g2_DrawCircle(&u8g2, 100, 10, 8, U8G2_DRAW_ALL);
		}
#endif
		/* speed triggers */
		_draw_speed_tirggers_list(90, 10, _speed_triggers, _speed_triggers_cnt);

		u8g2_UpdateDisplay(&u8g2);

//		static uint8_t i = 0;
//#if 0
//		u8g2_ClearBuffer(&u8g2);
//		u8g2_DrawBox(&u8g2, 0, 24, i%128, 8);
//		u8g2_DrawFrame(&u8g2, 0, 24, 128, 8);
//		u8g2_UpdateDisplayArea(&u8g2, 0/8, 24/8, 128/8, 8/8);
////		u8g2_UpdateDisplay(&u8g2);
//#endif
////		u8g2_DrawCircle(&u8g2, 64, 32, (i%20)+5, U8G2_DRAW_ALL);
//
//		//		u8g2_UpdateDisplayArea(&u8g2, 0, 26, 128, 8);
////		u8g2_UpdateDisplay(&u8g2);
//
//
//
//#if 0
//		u8g2_DrawBox(&u8g2, 0, 36, (i+32)%128, 24);
//		u8g2_DrawFrame(&u8g2, 0, 36, 128, 24);
//
//		u8g2_UpdateDisplay(&u8g2);
//#endif
//		i++;


		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
};

int32_t display_init(void)
{
	xTaskCreate(&task_lcd, "lcd", 2048, NULL, 10, NULL);

	return 0;
}

