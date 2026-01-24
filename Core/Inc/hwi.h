/*
 * hwi.h
 *
 *  Created on: Jan 21, 2026
 *      Author: MINH HUNG
 */
#include "usart.h"
#include "main.h"
#include "software_timer.h"

#ifndef INC_HWI_H_
#define INC_HWI_H_

#define MASTER_MODE

#define MAX_BUFFER 256

extern uint8_t rx[MAX_BUFFER];
extern uint8_t tx[MAX_BUFFER];

extern uint8_t rx_index;
extern uint8_t read_index;

void hw_init();
uint8_t send_byte(uint8_t data);
uint8_t receive_byte(uint8_t *data);
void UART_Callback(UART_HandleTypeDef *huart);

#endif /* INC_HWI_H_ */
