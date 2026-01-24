#include "hwi.h"
#include "main.h"

uint8_t received_byte;
uint8_t rx_index = 0;
uint8_t read_index = 0;
uint8_t rx[MAX_BUFFER];

void hw_init(){
    HAL_UART_Receive_IT(&huart3, &received_byte, 1);
    #ifdef MASTER_MODE
        HAL_GPIO_WritePin(EN_RS485_GPIO_Port, EN_RS485_Pin, GPIO_PIN_SET);
    #else
        HAL_GPIO_WritePin(EN_RS485_GPIO_Port, EN_RS485_Pin, GPIO_PIN_RESET);
    #endif
}

uint8_t send_byte(uint8_t data){
	if(HAL_UART_Transmit(&huart3, &data, 1, 0xFFFFFU) == HAL_OK) return 1;
	return 0;
}

uint8_t receive_byte(uint8_t *data){
	if(read_index == rx_index) return 0;
	*data = rx[read_index++];
	if(read_index >= MAX_BUFFER) read_index = 0;
	return 1;
}

void UART_Callback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART3){
		rx[rx_index++] = received_byte;
		if(rx_index >= MAX_BUFFER) rx_index = 0;
		HAL_UART_Receive_IT(huart, &received_byte, 1);
	}
}
