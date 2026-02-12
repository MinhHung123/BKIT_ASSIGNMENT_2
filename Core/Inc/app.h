/*
 * app.h
 *
 *  Created on: Feb 2, 2026
 *      Author: MINH HUNG
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#define DATA_ONLY 	0x00
#define ENCRYPTED 	0x01
#define COMMAND 	0x02
#define PRIORITY 	0x04

#include "hwi.h"
#include "mpl.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "sensor.pb.h"
#include "lcd.h"
#include "crc16.h"

void master_encode(uint32_t slave_id, float temp, float humid);
void slave_decode(uint8_t *ptr, uint16_t pay_length);
void update_sensor_data();

#endif /* INC_APP_H_ */
