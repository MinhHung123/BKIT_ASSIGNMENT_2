#include "hwi.h"
#include "main.h"

uint8_t received_byte;
uint8_t rx_index = 0;
uint8_t read_index = 0;
uint8_t rx[MAX_BUFFER];
ModeCommu current_mode = MODE_UART;

void hw_init(){
    HAL_UART_Receive_IT(&huart3, &received_byte, 1);
    #ifdef MASTER_MODE
        HAL_GPIO_WritePin(EN_RS485_GPIO_Port, EN_RS485_Pin, GPIO_PIN_SET);
    #else
        HAL_GPIO_WritePin(EN_RS485_GPIO_Port, EN_RS485_Pin, GPIO_PIN_RESET);
        HAL_I2C_EnableListen_IT(&hi2c1);
    #endif
}

uint8_t send_byte(uint8_t data){
	if(current_mode == MODE_UART){
		return (HAL_UART_Transmit(&huart3, &data, 1, 0xFFFFFU) == HAL_OK);
	}else{
		return (HAL_I2C_Master_Transmit(&hi2c1, 0x01, &data, 1, 0xFFFFFU) == HAL_OK);
	}
}

uint8_t receive_byte(uint8_t *data){
	if(read_index == rx_index) return 0;
	*data = rx[read_index++];
	if(read_index >= MAX_BUFFER) read_index = 0;
	return 1;
}

void UART_Callback(UART_HandleTypeDef *huart){
	if(current_mode == MODE_UART){
		if(huart->Instance == USART3){
			rx[rx_index++] = received_byte;
			if(rx_index >= MAX_BUFFER) rx_index = 0;
			HAL_UART_Receive_IT(huart, &received_byte, 1);
		}
	}
}

void I2C_Callback(I2C_HandleTypeDef *hi2c){
	if(current_mode == MODE_I2C){
		if(hi2c->Instance == hi2c1.Instance){
			rx[rx_index++] = received_byte;
			if(rx_index >= MAX_BUFFER) rx_index = 0;
			HAL_I2C_Slave_Receive_IT(hi2c, &received_byte, 1);
		}
	}
}

void convert_mode(){
	if(isPress(0)){
		if(current_mode == MODE_UART){
			current_mode = MODE_I2C;
			HAL_UART_AbortReceive(&huart3);
			HAL_I2C_Slave_Receive_IT(&hi2c1, &received_byte, 0xFFFFU);
			lcd_clear(BLACK);
			lcd_show_string(10, 100, "MODE: I2C", YELLOW, BLACK, 16, 0);
		}
		else{
			current_mode = MODE_UART;
			HAL_I2C_Master_Abort_IT(&hi2c1, 0x01);
			HAL_UART_Receive_IT(&huart3, &received_byte, 1);
			lcd_clear(BLACK);
			lcd_show_string(10, 100, "MODE: UART", YELLOW, BLACK, 16, 0);
		}
	}
}
