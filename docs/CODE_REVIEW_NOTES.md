# 코드 점검 메모

환경을 만들면서 clang 경고와 clang-tidy가 짚은 곳 중, 실제 버그일 가능성이 있는 것을 정리했다. **`src/`는 수정하지 않았다.** 확실도는 읽은 코드 기준이며 보드에서 재현해 확인한 것은 아니다.

## 가능성이 높은 것

### 1. `modbusTransmit()`의 `n` 미초기화 — `src/hw/driver/modbus.c:250, 259, 311`
`uint8_t i,n,func,qty;`로 선언만 하고, `switch(gModBus.mode)`에는 `default`가 없다. `mode`가 세 케이스(COMMAND, MONITOR, 세 번째) 밖의 값이면 `n`이 쓰레기 값인 채로 `if(n > 0)`로 들어가 `tb[]` 버퍼를 임의 길이만큼 RS485로 송신한다. 커밋 기록의 "modbus bug patch", "변수 오염"과 증상이 맞을 수 있다.
확인 방법: `n`을 0으로 초기화하거나 `default: n = 0;`을 추가한 뒤 증상이 사라지는지 본다.

### 2. UART4(SC3, Modbus) 송신 대기 조건 — `src/hw/driver/uart.c:140`
```c
while(SC3STR & 0x20 == 0x20)      // SC3STR & (0x20 == 0x20) -> SC3STR & 1
```
같은 파일의 132, 136행은 `(SC1STR & 0x20) == 0x20`로 괄호가 있다. 140행만 괄호가 없어 비트 5 대신 비트 0을 검사한다. 루프에 30ms 타임아웃(`return 0`)이 있어서 오동작이 겉으로는 드러나지 않을 수 있다.
수정 방향: `while((SC3STR & 0x20) == 0x20)`.

## 가능성이 있는 것

| 위치 | 내용 |
|---|---|
| `src/hw/driver/MAX6675.c:313` | `sum = (sum - max - min)/(length - 2);` — `length == 2`이면 0으로 나눈다. 호출부에서 `length >= 3`이 보장되는지 확인 필요 |
| `src/hw/driver/eeprom.c:118, 277` | 초기화되지 않은 값이 반환/배열 첨자로 쓰이는 경로가 있다는 정적 분석 지적(clang-analyzer) |
| `src/hw/driver/cli.c:244` | `uartWrite(cli_node.ch, (uint8_t)p_data)` — 포인터를 1바이트로 잘라 쓴다. `cliWrite()`는 호출하는 곳이 없어 지금은 영향이 없다 |
| `src/hw/driver/i2c.c:256, 295` | 루프 변수가 `uint8_t`인데 상한이 `uint32_t`. 상한이 255를 넘으면 무한 루프 |

## 의도된 것으로 보이는 것(참고)

- `src/hw/driver/timer.c:121`: `static pAccTime = 20;` — 형 없는 선언(암묵적 int). 구형 컴파일러는 허용한다.
- `src/hw/driver/eeprom.c:245`: `*Ptr++;` — 역참조 결과를 버리고 포인터만 증가한다.
- `src/hw/driver/cli.c:576`: `argv[argc] = '\0';` — 널 포인터 대입을 문자 상수로 쓴 것.
- clang-tidy의 `uart.c:213~240` ArrayBound 6건은 `fmt` 버퍼 분석이며 오탐일 수 있다. 확인 전에는 단정하지 않는다.
