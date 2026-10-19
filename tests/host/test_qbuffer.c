/* src/common/qbuffer.c (링 버퍼) 검증. 하드웨어 의존이 없어서 소스를 그대로 링크한다. */
#include "test.h"
#include "qbuffer.h"

TEST(create_starts_empty)
{
    uint8_t mem[8];
    qbuffer_t q;
    qbufferCreate(&q, mem, 8);
    CHECK_EQ(qbufferAvailable(&q), 0);
}

TEST(write_then_read_keeps_order)
{
    uint8_t mem[8], in[3] = {10, 20, 30}, out[3] = {0};
    qbuffer_t q;
    qbufferCreate(&q, mem, 8);
    CHECK_EQ(qbufferWrite(&q, in, 3), 1);
    CHECK_EQ(qbufferAvailable(&q), 3);
    CHECK_EQ(qbufferRead(&q, out, 3), 1);
    CHECK_EQ(out[0], 10);
    CHECK_EQ(out[1], 20);
    CHECK_EQ(out[2], 30);
    CHECK_EQ(qbufferAvailable(&q), 0);
}

TEST(capacity_is_len_minus_one)
{
    /* 링 버퍼 구현상 in==out이 '빔'이라서 len=4 이면 3개까지만 들어간다. */
    uint8_t mem[4], in[4] = {1, 2, 3, 4};
    qbuffer_t q;
    qbufferCreate(&q, mem, 4);
    CHECK_EQ(qbufferWrite(&q, in, 4), 0);
    CHECK_EQ(qbufferAvailable(&q), 3);
}

TEST(read_from_empty_reports_failure)
{
    uint8_t mem[4], out[1];
    qbuffer_t q;
    qbufferCreate(&q, mem, 4);
    CHECK_EQ(qbufferRead(&q, out, 1), 0);
}

TEST(wraps_around_end_of_buffer)
{
    uint8_t mem[4], a[3] = {1, 2, 3}, b[2] = {4, 5}, out[3] = {0};
    qbuffer_t q;
    qbufferCreate(&q, mem, 4);
    qbufferWrite(&q, a, 3);
    qbufferRead(&q, out, 2);          /* 1, 2 소비 -> 남은 것 3 */
    CHECK_EQ(qbufferWrite(&q, b, 2), 1); /* 4, 5 기록: 인덱스가 끝에서 되돌아옴 */
    CHECK_EQ(qbufferAvailable(&q), 3);
    qbufferRead(&q, out, 3);
    CHECK_EQ(out[0], 3);
    CHECK_EQ(out[1], 4);
    CHECK_EQ(out[2], 5);
}

TEST(flush_discards_everything)
{
    uint8_t mem[4], in[2] = {1, 2};
    qbuffer_t q;
    qbufferCreate(&q, mem, 4);
    qbufferWrite(&q, in, 2);
    qbufferFlush(&q);
    CHECK_EQ(qbufferAvailable(&q), 0);
}

TEST(multi_byte_elements)
{
    uint8_t mem[8], in[4] = {0xAA, 0xBB, 0xCC, 0xDD}, out[4] = {0};
    qbuffer_t q;
    qbufferCreateBySize(&q, mem, 2, 4); /* 원소 크기 2바이트, 4칸 */
    CHECK_EQ(qbufferWrite(&q, in, 2), 1);
    CHECK_EQ(qbufferAvailable(&q), 2);
    CHECK_EQ(qbufferRead(&q, out, 2), 1);
    CHECK(memcmp(in, out, 4) == 0);
}

int main(void)
{
    RUN(create_starts_empty);
    RUN(write_then_read_keeps_order);
    RUN(capacity_is_len_minus_one);
    RUN(read_from_empty_reports_failure);
    RUN(wraps_around_end_of_buffer);
    RUN(flush_discards_everything);
    RUN(multi_byte_elements);
    return TEST_SUMMARY();
}
