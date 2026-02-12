/*
 * mpl.c
 *
 *  Created on: Jan 27, 2026
 *      Author: MINH HUNG
 */
#include "mpl.h"

#define MAX_BUFFER_LENGTH 256

uint8_t receive_buffer[MAX_BUFFER_LENGTH] = {0x00};
uint8_t buffer_index = 0;
uint16_t packet_length = 0;
uint8_t flag = 0x00;
uint8_t new_byte = 0x00;

enum mpl_state {IDLE, RECEIVE_HEADER, RECEIVE_PAYLOAD, CRC_CHECKING};

uint8_t mpl_status = IDLE;
// ctdl sau khi decode

void mpl_fsm(){
	// receiving new byte
	if(receive_byte(&new_byte) == 0) return;
	// fsm
	switch (mpl_status){
	case IDLE:
		// TO DO
		if(new_byte == 0x7E){
			mpl_init(); // Gọi init TRƯỚC khi lưu byte đầu tiên
			receive_buffer[buffer_index++] = new_byte; // Lưu 0x7E vào index 0
			mpl_status = RECEIVE_HEADER;
			timer_set(0, 1500);
		}
		break;
	case RECEIVE_HEADER:
		// TO DO
		if(receiving_header()){
			timer_set(0, 1500);
			mpl_status = RECEIVE_PAYLOAD;
		}
		if(timer_isExpired(0)){
			mpl_status = IDLE;
		}
		break;
	case RECEIVE_PAYLOAD:
		// TO DO
		if(receiving_payload()){
			timer_set(0, 1500);
			mpl_status = CRC_CHECKING;
		}
		if(timer_isExpired(0)){
			mpl_status = IDLE;
		}
		break;
	case CRC_CHECKING:
		// TO DO
		if(crc_checking()){
			slave_decode(&receive_buffer[6], packet_length);
			lcd_show_string(100, 180, "packet valid  ", GREEN, BLACK, 16, 0);
		}else {
            lcd_show_string(100, 180, "packet invalid", RED, BLACK, 16, 0);
        }
		mpl_status = IDLE;
		break;
	default:
		break;
	}
}

void mpl_init(){
    for(uint16_t i=0; i<MAX_BUFFER_LENGTH; i++){
        receive_buffer[i] = 0x00;
    }
    buffer_index = 0;
    packet_length = 0;
}

uint8_t receiving_header(){
	receive_buffer[buffer_index++] = new_byte;
	// packet header
	if(buffer_index == 6){
		// get frame length
		packet_length = ((uint16_t)receive_buffer[1] << 8) | (uint16_t)receive_buffer[2];
		// get flag
		flag = receive_buffer[5];
		return 1;
	}
	return 0;
}

uint8_t receiving_payload(){
	receive_buffer[buffer_index++] = new_byte;
	if(buffer_index == 6 + packet_length){
		return 1;
	}
	return 0;
}

uint8_t crc_checking(){
	if(buffer_index >= 6){
		uint16_t crc_calcualated = crc16(receive_buffer, buffer_index);
		uint8_t crc_high = (crc_calcualated >> 8) & 0xFF;
		uint8_t crc_low = crc_calcualated & 0xFF;
		if(crc_low == receive_buffer[4] && crc_high == receive_buffer[3]){
			return 1;
		}
	}
	return 0;
}
