/* src/hw/driver/modbus.c 의 CRC16(표 방식) 검증.
 * modbusTBL 이 static 이라서 소스를 직접 #include 한다. 하드웨어 의존 함수는 stubs.c가 대신한다.
 * 기준값은 Modbus RTU 표준 CRC-16(다항식 0xA001, 초기값 0xFFFF)을 비트 연산으로 계산한 것이다. */
#include "test.h"
#include "../../src/hw/driver/modbus.c"

static unsigned short reference_crc16(const uint8_t *p, int len)
{
    unsigned short crc = 0xFFFF;
    int i, b;
    for (i = 0; i < len; i++) {
        crc ^= p[i];
        for (b = 0; b < 8; b++)
            crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : crc >> 1;
    }
    return crc;
}

/* CRC16()은 결과를 modbusTBL.cm3.hi / .low 에 넣는다. 이름과 달리 .hi 는 '먼저 전송되는' 바이트(표준 CRC 값의 하위 바이트),
 * .low 는 '나중에 전송되는' 바이트(상위 바이트)다. modbusTransmit()이 tb[6]=hi, tb[7]=low 순으로 보내므로
 * 전송 순서는 Modbus 표준(하위 바이트 먼저)과 같다. 그래서 표준 CRC 값 = (low << 8) | hi 이다. */
static unsigned short table_crc16(uint8_t *msg, uint8_t len)
{
    CRC16(msg, len);
    return (unsigned short)((modbusTBL.cm3.low << 8) | modbusTBL.cm3.hi);
}

TEST(known_vector_read_holding_registers)
{
    /* 01 03 00 00 00 0A -> 표준 CRC 0xCDC5 (전송 순서는 C5 CD) */
    uint8_t msg[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
    CHECK_EQ(reference_crc16(msg, 6), 0xCDC5);
    CHECK_EQ(table_crc16(msg, 6), 0xCDC5);
}

TEST(wire_order_is_low_byte_first)
{
    /* 표준 CRC 0xCDC5 는 선로에서 C5 CD 순서로 나가야 한다. 펌웨어는 tb[6]=hi, tb[7]=low 로 보낸다. */
    uint8_t msg[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
    CRC16(msg, 6);
    CHECK_EQ(modbusTBL.cm3.hi, 0xC5);
    CHECK_EQ(modbusTBL.cm3.low, 0xCD);
}

TEST(matches_reference_for_various_lengths)
{
    uint8_t msg[32];
    int i, len;
    for (i = 0; i < 32; i++) msg[i] = (uint8_t)(i * 37 + 11);
    for (len = 1; len <= 32; len++)
        CHECK_EQ(table_crc16(msg, (uint8_t)len), reference_crc16(msg, len));
}

TEST(command_frame_used_by_firmware)
{
    /* modbusTransmit()가 만드는 쓰기 프레임: ID=3, 기능코드 0x06, 주소, 값 */
    uint8_t msg[] = {0x03, 0x06, 0x00, 0x01, 0x00, 0x02};
    CHECK_EQ(table_crc16(msg, 6), reference_crc16(msg, 6));
}

TEST(zero_length_leaves_initial_value)
{
    uint8_t msg[1] = {0};
    CHECK_EQ(table_crc16(msg, 0), 0xFFFF);
}

int main(void)
{
    RUN(known_vector_read_holding_registers);
    RUN(wire_order_is_low_byte_first);
    RUN(matches_reference_for_various_lengths);
    RUN(command_frame_used_by_firmware);
    RUN(zero_length_leaves_initial_value);
    return TEST_SUMMARY();
}
