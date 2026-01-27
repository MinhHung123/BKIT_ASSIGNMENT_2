/*
 * mpl.h
 *
 *  Created on: Jan 27, 2026
 *      Author: MINH HUNG
 */

#ifndef INC_MPL_H_
#define INC_MPL_H_

#include "hwi.h"
#include "crc16.h"
#include "software_timer.h"
#include "lcd.h"

void mpl_fsm();
void mpl_init();
uint8_t is_start_byte();
uint8_t receiving_header();
uint8_t receiving_payload();
uint8_t crc_checking();

#endif /* INC_MPL_H_ */
