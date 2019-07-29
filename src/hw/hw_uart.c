#include "hw_uart.h"
#include "hw_board.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "soc/gpio_struct.h"

#define BUF_SIZE (1024)

#define DBG_UART_NUMBER UART_NUM_1

void uart_debug_init()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(DBG_UART_NUMBER, &uart_config);
    //uart_set_pin(UART_NUM_1, DEBUG_TX, DEBUG_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_set_pin(DBG_UART_NUMBER, DEBUG_TX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(DBG_UART_NUMBER, BUF_SIZE * 2, 0, 0, NULL, 0);

    /*
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
    	// Write data back to the UART
        uart_write_bytes(UART_NUM_1, (const char *) data, len);

    	vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    */
}

void dbg_uart_sleep(void)
{
//	uart_flush(DBG_UART_NUMBER);

//	uart_driver_delete(DBG_UART_NUMBER);

    /* set pull up and disable pin */
    gpio_reset_pin(DEBUG_TX);
}

void dbg_uart_wakeup(void)
{
    const gpio_config_t cfg = {
        .pin_bit_mask = BIT64(DEBUG_TX),
        .mode = GPIO_MODE_DISABLE,
        .pull_up_en = false,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&cfg);
    uart_set_pin(DBG_UART_NUMBER, DEBUG_TX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

//	uart_debug_init();
}
