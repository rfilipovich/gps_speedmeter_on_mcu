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

extern volatile uint32_t for_one_handred_ms;

void task_lcd(void *ignore)   {

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

	bool _blink_activity = false;
	for(;;) {
		u8g2_ClearBuffer(&u8g2);

		{/* draw speed */
			char speed_str[16] = {0};
			sprintf(speed_str, "%03d",
					(ext_speed/1000)%1000
					/*, (ext_speed/100)%10*/);
			
			u8g2_SetFont(&u8g2, u8g2_font_fub35_tf);
			u8g2_DrawStr(&u8g2, 2, 40, speed_str);
		};

	
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

		{///// to 1 hundred ///
			uint32_t showed_to_hundred = for_one_handred_ms/10;
			u8g2_SetFont(&u8g2, u8g2_font_courB08_tf);
			
			if(for_one_handred_ms != 0) {
				if(showed_to_hundred > 9999) {
					showed_to_hundred = 9999;
				};
			
				char speed_str[16] = {0};
				sprintf(speed_str, "%02d.%02d",
					(showed_to_hundred/100)%100,
					showed_to_hundred%100);
				u8g2_DrawStr(&u8g2, 90, 30, speed_str);
			} else {
				u8g2_DrawStr(&u8g2, 90, 30, "--.--");
			};
		};

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

