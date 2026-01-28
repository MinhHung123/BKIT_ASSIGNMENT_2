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

enum mpl_state {INIT, IDLE, RECEIVE_HEADER, RECEIVE_PAYLOAD, CRC_CHECKING};
enum packet_state {OK, NOK};

uint8_t mpl_status = INIT;
uint8_t packet_status = NOK;
// ctdl sau khi decode
SensorMessage decode_msg = SensorMessage_init_default;

void mpl_fsm(){
	// receiving new byte
	if(receive_byte(&new_byte) == 0) return;
	// fsm
	switch (mpl_status){
	case IDLE:
		// TO DO
		if(is_start_byte()){
			mpl_init();
			mpl_status = RECEIVE_HEADER;
		}
		break;
	case RECEIVE_HEADER:
		// TO DO
		if(receiving_header()){
			mpl_status = RECEIVE_PAYLOAD;
		}
		break;
	case RECEIVE_PAYLOAD:
		// TO DO
		if(receiving_payload()){
			timer_set(0, 1500);
			mpl_status = CRC_CHECKING;
		}
		break;
	case CRC_CHECKING:
		// TO DO
		if(crc_checking()){
			slave_decode(&receive_buffer[6], packet_length);
			packet_status = OK;
		}
		if(packet_status == OK){
			lcd_show_string(100, 200, "packet valid", GREEN, BLACK, 16, 0);
		} else{
			lcd_show_string(100, 200, "packet invalid", RED, BLACK, 16, 0);
		}
		if(timer_isExpired(0)){
			mpl_status = IDLE;
		}
		break;
	default:
		break;
	}
}

void mpl_init(){
	// clear buffer
	for(uint8_t i=0; i<MAX_BUFFER_LENGTH; i++){
		receive_buffer[i] = 0x00;
	}
	// reset header
	buffer_index = 0;
	packet_length = 0;
	packet_status = NOK;
}

uint8_t is_start_byte(){
	// check is start byte
	if(new_byte == 0x7E){
		receive_buffer[buffer_index++] = new_byte;
		return 1;
	}
	return 0;
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

void master_encode(uint32_t slave_id, float temp, float humid){
	//lcd_show_string(100, 200, "MASTER sending require", WHITE, BLACK, 16, 0);
	SensorMessage msg = SensorMessage_init_default;
	msg.device_id = slave_id;
	msg.temperature = temp;
	msg.humidity = humid;

	uint8_t payload_tmp[128];
    pb_ostream_t ostream = pb_ostream_from_buffer(payload_tmp, sizeof(payload_tmp));

	if(pb_encode(&ostream, SensorMessage_fields, &msg)){
		uint16_t len = (uint16_t)ostream.bytes_written;
		// packet dùng để gửi đi thông qua uart
		uint8_t tx_buf[128];
		// Header
		tx_buf[0] = 0x7E;
		tx_buf[1] = (uint8_t)(len >> 8);
		tx_buf[2] = (uint8_t)(len & 0xFF);
		tx_buf[5] = 0x00;
		// Chép payload
		for(uint8_t i=0; i<len; i++){
			tx_buf[6+i] = payload_tmp[i];
		}
		// Tính crc
		uint16_t crc_val = crc16(tx_buf, 6 + len);
		tx_buf[3] = (uint8_t)(crc_val >> 8);
		tx_buf[4] = (uint8_t)(crc_val & 0xFF);
		// Truyền qua uart
		for(int i=0; i<(len+6); i++){
			send_byte(tx_buf[i]);
		}
	}

}

void slave_decode(uint8_t *ptr, uint16_t pay_length){
	pb_istream_t istream = pb_istream_from_buffer(ptr, pay_length);

	if(pb_decode(&istream, SensorMessage_fields, &decode_msg)){
		// Hiển thị thông số giải mã đc và in lên lcd
		lcd_show_string(10, 200, "slave ID: ", WHITE, BLACK, 16, 0);
		lcd_show_int_num(100, 200, decode_msg.device_id, 2, GREEN, BLACK, 16);

		lcd_show_string(10, 220, "TEMP/HUMI: ", WHITE, BLACK, 16, 0);
		lcd_show_float_num(100, 220, decode_msg.temperature, 1, YELLOW, BLACK, 16);
		lcd_show_float_num(160, 220, decode_msg.humidity, 1, CYAN, BLACK, 16);
	}else{
		lcd_show_string(10, 200, "SLAVE ENCODE FAIL", RED, BLACK, 16, 0);
	}
}
