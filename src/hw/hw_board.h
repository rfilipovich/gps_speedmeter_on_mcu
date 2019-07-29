#ifndef HW_BOARD_H
#define HW_BOARD_H


#include <stdint.h>


/**************** PINS ****************/

/* =============== Definitions for display =============== */
#define DISP_SCK				GPIO_NUM_18
#define DISP_MOSI				GPIO_NUM_23
#define DISP_SS					GPIO_NUM_5
#define DISP_PIN				GPIO_NUM_13
#define DISP_LIGHT				GPIO_NUM_14

/* ============= Definitions for debug UART ============== */
#define DEBUG_RX				GPIO_NUM_3
#define DEBUG_TX				GPIO_NUM_1


uint32_t hw_board_init(void);

int32_t hw_board_sleep(void);

#endif // HW_BOARD_H
