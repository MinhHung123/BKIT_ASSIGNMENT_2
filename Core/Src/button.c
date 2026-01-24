/*
 * button.c
 *
 *  Created on: Oct 18, 2025
 *      Author: Dell
 */

/* Includes */
#include "button.h"

#include "spi.h"
#include "gpio.h"

/* Variables */
uint16_t button_count[16] = {0};
static uint16_t button_spi_buffer = 0x0000;

/* Functions */
/**
 * @brief  	Init matrix button
 * @param  	None
 * @retval 	None
 */
void button_init() {
	HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, 1);
}

/**
 * @brief  	Scan matrix button
 * @param  	None
 * @note  	Call every 50ms
 * @retval 	None
 */
void button_scan() {
	HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, 0);
	HAL_GPIO_WritePin(BTN_LOAD_GPIO_Port, BTN_LOAD_Pin, 1);
	HAL_SPI_Receive(&hspi1, (void*) &button_spi_buffer, 2, 10);

	int button_index = 0;
	uint16_t mask = 0x8000;
	for (int i = 0; i < 16; i++) {
		if (i >= 0 && i <= 3) {
			button_index = i + 4;
		} else if (i >= 4 && i <= 7) {
			button_index = 7 - i;
		} else if (i >= 8 && i <= 11) {
			button_index = i + 4;
		} else {
			button_index = 23 - i;
		}
		if (button_spi_buffer & mask)
			button_count[button_index] = 0;
		else
			button_count[button_index]++;
		mask = mask >> 1;
	}
}

/**
 * @brief Kiểm tra xem nút nhấn có đang được nhấn hay không
 * @param index: Chỉ số của nút nhấn (0-15)
 * @retval 1 nếu được nhấn, 0 nếu không
 */
uint8_t isPress(uint8_t index) {
    if (index >= 16) return 0;
    // Dựa trên logic của button_scan: button_count tăng khi nhấn
    // Chúng ta coi là "nhấn" nếu count > 0 (thường là 1 để bắt sườn xuống)
    return (button_count[index] > 0);
}

/**
 * @brief Reset trạng thái của nút nhấn về 0
 * @param index: Chỉ số của nút nhấn
 */
void resetButton(uint8_t index) {
    if (index >= 16) return;
    button_count[index] = 0; // Đặt lại bộ đếm về 0
}

/**
 * @brief Kiểm tra nhấn giữ (nếu cần dùng)
 */
uint8_t isLongPress(uint8_t index) {
    if (index >= 16) return 0;
    // Ví dụ: Nhấn giữ là khi count vượt quá 40 (tương ứng 2 giây nếu quét 50ms)
    return (button_count[index] >= 40);
}
