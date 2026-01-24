/*
 * global.h
 *
 *  Created on: Oct 18, 2025
 *      Author: Dell
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"

#define BAUDRATE		9600
#define BITS_PER_CHAR   10

#define RS485_T15_TIMER 1
#define RS485_T15       2

#define RS485_T35_TIMER	2
#define RS485_T35		5

#ifndef RS485_EN_TX_LEVEL
#define RS485_EN_TX_LEVEL GPIO_PIN_SET
#endif
#ifndef RS485_EN_RX_LEVEL
#define RS485_EN_RX_LEVEL GPIO_PIN_RESET
#endif

#define ROLE_MASTER
#ifndef ROLE_MASTER
#define ROLE_SLAVE
#endif

#endif /* INC_GLOBAL_H_ */
