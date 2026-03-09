Message Processing Layer

 
IDLE: 
 - Clear buffer, index, packet lenght
 - Nếu nhận được byte 0x7E (byte bắt đầu của packet) thì chuyển qua Receive Header

Receive Header:
 - Tiếp tục nhận byte gửi từ master, nếu đủ số byte header (index = 6) thì chuyển qua Receive payload

Receive Payload:
 - Tiếp tục nhận các byte payload cho đến khi đủ byte (index = 6 + packet length) thì chuyển qua CRC checking
 
CRC Checking:
  - Sử dụng crc16 để check xem thông tin có hợp với packet không. Nếu đúng thì cho slave thực hiện giải mà và nếu không thì báo lỗi. Sau đó trở về trạng thái IDLE



Byte Offset	Field	Description
0	Start byte	Optional (e.g., 0x7E)
1–2	Packet Length	16-bit length field (little-endian)
3–4	CRC (16-bit)	CRC-16 result of payload bytes
5	Flags/Key	Optional security or type info
6+	Payload	Encoded data (Nanobp)

Hardware Interface Layering

1.	Uart

uint8_t send_byte(uint8_t data);
uint8_t receive_byte(uint8_t *data);

Sử dụng hàm send_byte và receive_byte để gửi và nhận dữ liệu mỗi byte 
2.	I2C
	
Application Layer

void master_encode(uint32_t slave_id, float temp, float humid);
void slave_decode(uint8_t *ptr, uint16_t pay_length);

Sử dụng nanopb để đóng gói dữ liệu thành dạng nhị phân giúp giảm kích thước gói tin và việc decode và encode diễn ra nhanh và ít tốn bộ nhớ hơn so với sử dụng dụng json hay xml
 





Các bước sử dụng nanobp:
B1: viết một file .proto chứa các dữ liệu cần gửi đi.
Ví du:
syntax = "proto3";
message SensorData {
  uint32 sensor_id = 1;
  float sensor_value = 2;
  uint64 timestamp = 3;
} 



B2: Sử dụng protoc để biên dịch file .proto các file .c và .h để  

B3: Gọi các hàm pb.encode() và pb.decode() để mã hóa và giải mã dữ liệu trong dự án 
