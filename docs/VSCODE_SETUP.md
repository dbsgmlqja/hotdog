# HOTDOG VSCode 개발환경

DebugFactory Builder 5(DFB5)로 하던 편집, 빌드, 검증을 VSCode로 옮긴 환경이다. 대상 MCU는 Nuvoton(구 Panasonic) MN103DF05K(AM32R-4), 컴파일러는 DFB5 내장 CC103S 5.4R4다.

## 상태 요약

| 기능 | 상태 | 비고 |
|---|---|---|
| 코드 탐색/진단(clangd) | 동작 확인 | 27개 파일 문법 오류 0건 |
| 정적 분석(clang-tidy) | 동작 확인 | 규칙 조정 후 46건 |
| ROM/RAM 리포트 | 동작 확인 | DFB5 로그와 같은 값(ROM 37,578 / RAM 5,708) |
| 호스트 유닛테스트 | 동작 확인 | qbuffer 21, Pid 9, Modbus CRC 38개 검사 전부 통과 |
| `.mak`과 소스 목록 비교 | 동작 확인 | `tools/check_sync.py` |
| VSCode 빌드(`tools/build.py`) | 동작 확인 | 전체 빌드 4초, **`Hotdog.hex`가 DFB5 빌드와 바이트 단위로 완전 일치**, 컴파일 오류가 문제 패널에 표시됨 |
| 빌드 + 보드 다운로드 + 실행(`tools/dfb5.py`) | 동작 확인(2026-10-20) | 보드에 다운로드되고 실행됨. `build/dfb5_session.log`에 `Load completed`가 남는 것까지 확인 |
| cppcheck | 미설치 | 설치 허가 필요 |

## 처음 한 번

1. VSCode에서 이 폴더를 연다. 권장 확장(clangd, Serial Monitor)을 설치한다. `ms-vscode.cpptools`는 clangd와 충돌하므로 쓰지 않는다.
2. `Ctrl+Shift+P` → `Tasks: Run Task` → **HOTDOG: compile_commands 생성**. `build/compile_commands.json`이 만들어지고 clangd가 읽는다.
3. DFB5 설치 경로가 다르면 `.vscode/settings.json`의 `hotdog.dfbRoot`를 고친다.

## 자주 하는 작업

| 하고 싶은 일 | 방법 |
|---|---|
| 빌드 | `Ctrl+Shift+B` (또는 `python tools/build.py`) |
| DFB5가 실행할 명령만 확인 | 태스크 "Build 명령만 보기" (`--dry-run`) |
| 소스를 DFB5에서 추가했을 때 | 태스크 "DFB5 설정과 소스 목록 비교" → 어긋나면 `tools/project.json` 수정 |
| ROM/RAM 사용량 | `python tools/memreport.py` (`--save`로 이력 저장, 다음부터 변화량 표시) |
| 유닛테스트 | `python tests/run_tests.py` |
| 정적 분석 | `clang-tidy -p build <파일>` (clangd 안에서는 자동) |

## 구성 파일

```
tools/project.json            빌드 정의(소스, 링크 순서, 플래그, include). DFB5 .mak를 옮겨 적은 것
tools/build.py                cc103s/as103s/ld103s/xcv103 호출. build/ 에서 DF/ 와 같은 상대경로로 실행
tools/gen_compile_commands.py clangd/clang-tidy용 build/compile_commands.json 생성
tools/check_sync.py           DF/BAKERY_DF05.mak 와 project.json 비교
tools/memreport.py            .map 으로 ROM/RAM 요약
tests/run_tests.py            호스트(PC) 유닛테스트 실행기 (tests/host/*)
.clangd  .clang-tidy          clangd 와 clang-tidy 설정
.vscode/                      settings, tasks, extensions
```

## 알아 둘 점

- **DFB5와 병행한다.** `DF/` 폴더는 DFB5 전용으로 두고, VSCode 산출물은 `build/`에 만든다. 소스를 추가하거나 지울 때는 DFB5 프로젝트와 `tools/project.json` 양쪽을 맞춘다(`check_sync.py`로 확인).
- **`DF05_Hotdog.dfp5`의 hex 출력 경로를 고쳤다(2026-10-19).** 예전에는 `FLAGS_XCV`가 존재하지 않는 `C:\Users\HUIBEOM\Documents\fw\HOTDOG\OUTPUT\Hotdog.hex`를 가리켜서 DFB5 Make가 마지막 `XCV103` 단계에서 실패했다(F5001). 지금은 프로젝트 기준 상대경로 `..\OUTPUT\Hotdog.hex`다. `.dfp5`와 `DF/BAKERY_DF05.mak`에서 각각 한 줄만 바꿨고, 수정 전 파일은 `build/dfb5_reference/*.before_xcv_fix`에 있다. `.dfp5`에는 `WAFFLE_temp` 경로와 `FND.c` 같은 옛 항목이 아직 남아 있지만 DFB5가 걸러내고 있어서 그대로 두었다.
- **`DF/BAKERY_DF05.map`은 git에서 추적하지 않는다.** 빌드할 때마다 컴파일러 임시 파일 이름 등으로 2만 6천 줄이 바뀌어서 diff가 항상 지저분했다. 파일은 로컬에 그대로 있고, ROM/RAM 변화는 `tools/memreport.py`로 본다.
- **`int.c`는 컴파일되지만 링크되지 않는다.** DFB5의 `.mak`이 그렇게 되어 있어서 그대로 따랐다. 링크 순서도 컴파일 순서와 다르다.
- **빌드에 포함되지 않는 파일**이 있다: `bsp/cmsis/vector.c`(vect.c와 심볼 중복), `hw/driver/{int,hx711,sic2c,spi,sw_timer,FND}.c`. clangd 진단을 꺼 두었다.
- **파일 인코딩이 섞여 있다.** ASCII 61개, cp949 8개(buzzer, MAX6675, minit, Pid, swtimer, sw_timer, vect, vector), UTF-8 2개(display.c, modbus.c). 한글 주석이 있는 파일을 저장하기 전에 상태표시줄의 인코딩이 맞는지 확인한다. 도구가 cp949 파일을 UTF-8로 읽으면 깨진다.
- 컴파일러(`cc103s`)는 CP932(일본어 코드페이지)로 출력한다. `build.py`가 디코딩한다.
- clangd는 다른 컴파일러(clang, i386 타깃, `-fpack-struct=1`)로 소스를 해석하므로 `#pragma _section` 같은 독자 문법은 무시한다. 실제 빌드 결과와 다를 수 있다.

## DFB5 빌드와의 차이

2026-10-19에 같은 소스로 만든 결과를 DFB5(2026-10-06 빌드, `build/dfb5_reference/`에 백업)와 비교했다.

- `Hotdog.hex`: 105,753바이트 **완전 일치**. 코드와 데이터가 같다는 뜻이다.
- ROM 37,578B / RAM 5,708B: 같다.
- `.ro`는 파일마다 정확히 +3바이트, `.x`는 +81바이트(27개 × 3): 디버그 정보에 컴파일 폴더 이름이 들어가는데 `DF`가 `build`로 바뀐 것이다(2자 → 5자). 코드와 무관하다.
- `.map`: 차이는 컴파일러 임시 파일 이름(매번 무작위), 디버그 라인정보 라벨(`.LI_*`), 디버그 섹션 크기뿐이다.
- 기본 `Build`는 `OUTPUT/Hotdog.hex`를 갱신한다. 비교 등으로 덮어쓰고 싶지 않으면 `python tools/build.py --hex build/Hotdog_vscode.hex`처럼 경로를 따로 준다.
- 컴파일 오류 형식(문제 패널 연동용): C는 `파일:줄:(E|W|I)번호 메시지`, 어셈블러는 `파일:줄:as103S:(E|W번호):메시지`. 링커 오류 형식은 아직 확인하지 못했다.

## 빌드가 차단될 때 (Smart App Control)

> 2026-10-19에 사용자가 Smart App Control을 끈 뒤 위 빌드가 정상 동작했다. 아래는 다시 켜졌거나 다른 PC에서 같은 증상이 나올 때를 위한 설명이다.

이 PC에서는 Windows **Smart App Control**이 서명 없는 `cmp103s.exe`(컴파일러 본체)의 실행을 막는다(Win32 오류 4551, 이벤트 로그 Code Integrity 3077/3033). `as103s`, `ld103s`, `DFMake`는 실행되고 컴파일 단계만 막힌다. **DFB5 GUI의 Make도 같은 오류(`Create process error ... cmp103s.exe`)로 실패하는 것을 확인했다(2026-10-19).** 그래서 VSCode만의 문제가 아니라 이 PC 전체에서 C 컴파일이 안 된다. 10월 6일에는 DFB5 빌드가 됐으므로 그 사이에 Smart App Control이 평가 모드에서 강제 모드로 바뀐 것으로 보인다.

- `build.py`는 시작할 때 `cmp103s.exe`를 점검하고, 막혀 있으면 안내 메시지를 내고 종료 코드 2로 끝난다.
- 해결 방법: (1) Smart App Control 끄기, (2) Smart App Control이 꺼진 다른 PC에서 빌드, (3) IT에 `cmp103s.exe` 허용 요청(sha256 `8466B4F3E8BDA58A730DBFF36565B43785970F0B6C99D1F950171DC308EA3BA4`).
- Smart App Control 끄는 위치: Windows 보안 → 앱 및 브라우저 컨트롤 → 스마트 앱 컨트롤 설정. 예전에는 한 번 끄면 Windows를 다시 설치해야 켤 수 있었다. 커뮤니티 문서에 따르면 25H2 빌드 26200.8116 이상(이 PC는 26200.9457)에서는 재설치 없이 다시 켤 수 있다고 한다. 공식 문서로는 확인하지 못했으니, 끄기 전에 설정 화면에서 "켜기" 선택지가 같이 보이는지 확인한다.
- 호스트 유닛테스트용으로 만든 실행 파일도 간혹 막힌다. `-g`(디버그 정보)를 빼면 통과한 경우가 많았지만 보장되지는 않는다.

## 빌드 + 다운로드 + 실행 한 번에 하기 (`tools/dfb5.py`)

태스크 **HOTDOG: 빌드 + 보드 다운로드 + 실행 (DFB5)** 를 실행하면 아래를 순서대로 한다.

1. VSCode 빌드(오류를 문제 패널에 표시)
2. DFB5의 Make와 같은 `DFMake`로 `DF/BAKERY_DF05.x`, `OUTPUT/Hotdog.hex` 갱신
3. **선택창**: "보드에 다운로드합니다. 진행할까요?" (기본값은 '아니오'. '아니오'면 2까지만 하고 끝난다)
4. DFB5를 평소 쓰는 그룹(`DF/ProjectGroup5.dfgrp5`)으로 열고 매크로로 `DEBUGMODE /LOAD`(Flash에 기록) → `G`(실행). 다운로드 완료는 `build/dfb5_session.log`로 확인해 터미널에 보여 준다.

- 그 뒤 DFB5는 열려 있다. 중단, 스텝, 변수 확인은 DFB5 창에서 한다.
- **DFB5가 이미 열려 있으면 보드 단계는 건너뛴다**(같은 보드를 두 곳에서 쓸 수 없다). 1~2는 하므로 DFB5에서 Run만 누르면 된다.
- 다운로드만 하고 실행하지 않는 태스크("빌드 + 보드 다운로드 (실행은 안 함)")도 있다.
- 터미널에서: `python tools/dfb5.py run [--no-go] [--no-board] [--confirm yes]`. `--no-board`는 보드를 건드리지 않고 매크로만 만든다.
- 다운로드가 실패해도 매크로의 `G`는 실행될 수 있다(그러면 보드에 있던 이전 프로그램이 돈다). 그래서 성공 여부는 터미널의 "다운로드 완료를 확인했습니다" 문구로 판단한다.
- `DF/ProjectGroup5.dfgrp5`는 DFB5가 만드는 PC별 파일이다(git 제외). 없으면 `DF/DF05_Hotdog.dfp5`로 연다. 경로는 `tools/project.json`의 `dfb5`에 있다.

### 원리 (조사 결과)

- 기동: `DFB5.exe <프로젝트.dfp5> /C"<명령>"`. 예: `/C"DEBUGMODE /OPEN"`, `/C"<startup.mcr"`.
- 디버그 모드로 들어갈 때 현재 폴더의 `INIT.MCR`이 자동 실행된다.
- 자동화에 쓸 명령: `DEBUGMODE /OPEN|/CLOSE|/LOAD`, `L <파일.x>`(로드+리셋, `/NORESET`), `LV`(검증), `RESET`, `G`, `D`/`VAL`/`PF`(메모리/변수 출력), `><로그파일`(로깅), `WAIT`, `SYSTEM`, `EXIT!`.
- 이 기능들로 "VSCode 태스크 → 매크로 파일 생성 → DFB5 기동"을 구현했다. 변수 덤프(`D`, `VAL`, `PF`)는 아직 넣지 않았다. 스텝 디버깅은 DFB5 창에서 한다(이 칩은 GDB를 지원하지 않아 cortex-debug 같은 확장은 쓸 수 없다).
- 보드를 조작하는 작업(다운로드, 리셋, 실행)은 히터/모터/SSR이 실제로 움직일 수 있으니 실행 전에 반드시 확인한다.

## 시리얼

- CLI와 디버그 출력: Serial1(SC1), **115200bps**, FTDI USB 시리얼 `COM3`. VSCode Serial Monitor에서 COM3/115200으로 연다.
- Modbus는 Serial3(SC3, RS485)이다. 전송 속도는 코드에서 확정되지 않았다(약 9600bps로 추정).
