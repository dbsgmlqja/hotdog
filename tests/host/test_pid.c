/* src/hw/driver/Pid.c (getPidControl) 검증. 순수 계산이라 소스를 그대로 링크한다.
 * error = (setpoint - measurepoint) / 100 이고, value는 0 ~ basicLevel로 제한된다. */
#include "test.h"
#include "def.h"
#include "Pid.h"

static _PIDS make(double p, double i, double d, double setpoint, short measure, unsigned short level)
{
    _PIDS pid;
    memset(&pid, 0, sizeof pid);
    pid.P = p; pid.I = i; pid.D = d;
    pid.setpoint = setpoint;
    pid.measurepoint = measure;
    pid.basicLevel = level;
    return pid;
}

TEST(proportional_only)
{
    _PIDS pid = make(2.0, 0, 0, 1000, 0, 100);   /* error = 10, output = 20 */
    getPidControl(&pid);
    CHECK_EQ(pid.value, 20);
}

TEST(output_saturates_at_basic_level)
{
    _PIDS pid = make(50.0, 0, 0, 1000, 0, 100);  /* output = 500 > 100 */
    getPidControl(&pid);
    CHECK_EQ(pid.value, 100);
}

TEST(small_output_is_cut_to_zero)
{
    _PIDS pid = make(0.1, 0, 0, 1000, 0, 100);   /* output = 1 (< 2) */
    getPidControl(&pid);
    CHECK_EQ(pid.value, 0);
}

TEST(negative_error_gives_zero)
{
    _PIDS pid = make(2.0, 0, 0, 0, 1000, 100);   /* 측정값이 목표보다 높음 */
    getPidControl(&pid);
    CHECK_EQ(pid.value, 0);
}

TEST(integral_accumulates_while_below_limit)
{
    _PIDS pid = make(0, 1.0, 0, 1000, 0, 100);   /* error = 10 */
    getPidControl(&pid);
    CHECK_NEAR(pid.integral, 10.0, 1e-9);
    getPidControl(&pid);
    CHECK_NEAR(pid.integral, 20.0, 1e-9);
}

TEST(integral_frozen_when_output_at_limit)
{
    /* 안티 와인드업: value가 basicLevel 이상이면 적분을 더하지 않는다. */
    _PIDS pid = make(0, 1.0, 0, 1000, 0, 100);
    pid.value = 100;
    getPidControl(&pid);
    CHECK_NEAR(pid.integral, 0.0, 1e-9);
}

TEST(derivative_uses_previous_error)
{
    _PIDS pid = make(0, 0, 1.0, 1000, 0, 100);   /* D=1, 처음 error=10 -> derivate=10 */
    getPidControl(&pid);
    CHECK_EQ(pid.value, 10);
    getPidControl(&pid);                          /* error 동일 -> derivate=0 */
    CHECK_EQ(pid.value, 0);
}

int main(void)
{
    RUN(proportional_only);
    RUN(output_saturates_at_basic_level);
    RUN(small_output_is_cut_to_zero);
    RUN(negative_error_gives_zero);
    RUN(integral_accumulates_while_below_limit);
    RUN(integral_frozen_when_output_at_limit);
    RUN(derivative_uses_previous_error);
    return TEST_SUMMARY();
}
