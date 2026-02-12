#include "app.h"
#include "stdio.h"

SensorMessage decode_msg = SensorMessage_init_default;
extern uint16_t adc_buffer[2];
extern float temp;
extern float humid;

void master_encode(uint32_t slave_id, float temp, float humid){
    SensorMessage msg = SensorMessage_init_default;
    msg.device_id = slave_id;
    msg.temperature = temp;
    msg.humidity = humid;

    uint8_t payload_tmp[64]; // SensorMessage_size chỉ 24, dùng 64 là đủ
    pb_ostream_t ostream = pb_ostream_from_buffer(payload_tmp, sizeof(payload_tmp));

    if(pb_encode(&ostream, SensorMessage_fields, &msg)){
        uint16_t len = (uint16_t)ostream.bytes_written;
        uint8_t tx_buf[128] = {0}; // Khởi tạo bằng 0

        tx_buf[0] = 0x7E;
        tx_buf[1] = (uint8_t)(len >> 8);
        tx_buf[2] = (uint8_t)(len & 0xFF);
        tx_buf[5] = 0x00; // Flag

        for(uint8_t i=0; i<len; i++) tx_buf[6+i] = payload_tmp[i];

        uint16_t crc_val = crc16(tx_buf, 6 + len);
        tx_buf[3] = (uint8_t)(crc_val >> 8);
        tx_buf[4] = (uint8_t)(crc_val & 0xFF);

        for(int i=0; i<(len+6); i++) send_byte(tx_buf[i]);
    }
}

void slave_decode(uint8_t *ptr, uint16_t pay_length){
	pb_istream_t istream = pb_istream_from_buffer(ptr, pay_length);
	char display_buf[25];

	if(pb_decode(&istream, SensorMessage_fields, &decode_msg)){
		// Hiển thị thông số giải mã đc và in lên lcd
		sprintf(display_buf, "Slave ID: %02d  ", (int)decode_msg.device_id);
		lcd_show_string(10, 40, display_buf, WHITE, BLACK, 16, 0); //

		// Hiển thị Nhiệt độ (Lấy 2 chữ số thập phân)
		sprintf(display_buf, "TEMP: %.2f C  ", decode_msg.temperature);
		lcd_show_string(10, 60, display_buf, YELLOW, BLACK, 16, 0); //

		// Hiển thị Độ ẩm
		sprintf(display_buf, "HUMID: %.2f %%  ", decode_msg.humidity);
		lcd_show_string(10, 80, display_buf, CYAN, BLACK, 16, 0); //
	}else{
		lcd_show_string(10, 40, "SLAVE DECODE FAIL", RED, BLACK, 16, 0);
	}
}

void update_sensor_data(){
	temp = (adc_buffer[0] / 4095.0f) * 330.0f;
	humid = (adc_buffer[1] / 4095.0f) * 100.0f;
	master_encode(1, temp, humid);
}
