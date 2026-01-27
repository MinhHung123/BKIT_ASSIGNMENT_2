/*
 * crc16.h
 *
 *  Created on: Jan 26, 2026
 *      Author: MINH HUNG
 */

#ifndef INC_CRC16_H_
#define INC_CRC16_H_

#include <stdint.h>
#include <stdlib.h>

extern uint16_t lookup_table[256];

uint16_t crc16(const uint8_t *data, uint16_t length_bytes);

#endif /* INC_CRC16_H_ */
