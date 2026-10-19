/* PC(호스트)용 최소 테스트 도구. 일부러 <stdint.h>를 쓰지 않는다:
 * src/common/def.h가 uint8_t 등을 직접 typedef 해서(uint64_t = unsigned long) 같이 쓰면 충돌한다. */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>
#include <math.h>

static int g_checks, g_failures;

#define CHECK(cond) do { g_checks++; if (!(cond)) { g_failures++; \
    printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

#define CHECK_EQ(a, b) do { long long _a = (long long)(a), _b = (long long)(b); g_checks++; \
    if (_a != _b) { g_failures++; \
        printf("  FAIL %s:%d  %s == %s  (%lld != %lld)\n", __FILE__, __LINE__, #a, #b, _a, _b); } } while (0)

#define CHECK_NEAR(a, b, eps) do { double _a = (double)(a), _b = (double)(b); g_checks++; \
    if (fabs(_a - _b) > (eps)) { g_failures++; \
        printf("  FAIL %s:%d  %s ~= %s  (%g vs %g)\n", __FILE__, __LINE__, #a, #b, _a, _b); } } while (0)

#define TEST(name) static void name(void)
#define RUN(name) do { int _f0 = g_failures; name(); \
    printf("%s %s\n", g_failures == _f0 ? "[ OK ]" : "[FAIL]", #name); } while (0)
#define TEST_SUMMARY() (printf("%d checks, %d failures\n", g_checks, g_failures), g_failures ? 1 : 0)

#endif
