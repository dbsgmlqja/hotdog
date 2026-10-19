# HOTDOG 펌웨어 (MN103DF05K)

Nuvoton MN103DF05K(AM32R-4) 펌웨어. 컴파일러는 DFBuilder5 내장 CC103S 5.4R4(GCC 아님). 자세한 환경 설명은 `docs/VSCODE_SETUP.md`.

## 명령

```
python tools/gen_compile_commands.py   # 처음 한 번, 소스 목록이 바뀔 때 (build/compile_commands.json)
python tools/build.py [rebuild|clean] [--dry-run]
python tools/check_sync.py             # DF/BAKERY_DF05.mak 와 tools/project.json 비교
python tools/memreport.py [--save]     # ROM/RAM 요약
python tests/run_tests.py [이름]        # 호스트 유닛테스트 (PC에서 src/ 를 직접 컴파일)
python tools/dfb5.py run --no-board    # 빌드 + DFB5 Make + 매크로 생성까지만(보드 안 건드림). 보드 다운로드는 --confirm 없이는 터미널에서 묻는다
clang-tidy -p build <파일>
```

## 주의

- **`src/`는 요청 없이 수정하지 않는다.** 이 환경 구성은 소스를 건드리지 않는다는 전제로 만들었다. 버그를 발견하면 `docs/CODE_REVIEW_NOTES.md`에 기록하고 보고한다.
- **파일 인코딩이 섞여 있다**(cp949 8개, UTF-8 2개, 나머지 ASCII). 한글 주석 파일을 편집 도구로 덮어쓰면 깨진다. cp949 파일은 Edit/Write가 거부하므로 인코딩을 유지하는 방법으로 수정한다.
- **보드 조작(다운로드, 리셋, 실행)은 사용자 확인 후에만** 한다. 히터/모터/SSR이 실제로 동작할 수 있다.
- **Windows Smart App Control이 서명 없는 실행 파일을 막는다.** `cmp103s.exe`(컴파일러 본체)가 실행되지 않으면 빌드가 안 된다(오류 4551). DFB5 GUI도 같은 이유로 실패하는 것을 2026-10-19에 확인했다. 새 실행 파일을 만들거나 보안 설정을 바꾸는 일은 사용자 결정이다. 새 실행 파일을 만들어 반복 실행하는 실험은 보안 알림을 계속 띄우므로 하지 않는다.
- `.vscode/tasks.json`의 태스크는 모두 `"type": "process"`(인자 배열)로 만든다. `shell` 방식은 VSCode가 `pwsh -Command`로 실행하면서 `C:\Program Files (x86)\...` 경로의 따옴표를 벗겨 `x86: 용어를 인식할 수 없음` 오류가 난다(2026-10-20 실제 발생).
- 소스를 추가/삭제하면 DFB5 프로젝트(`DF/DF05_Hotdog.dfp5`)와 `tools/project.json` 양쪽을 맞춘다.
- 빌드 산출물(`*.ro`, `*.x`, `build/`)은 git에 올리지 않는다(`.gitignore`).
- 커밋은 사용자가 요청할 때만 한다. 작업트리에 아직 커밋하지 않은 사용자 수정이 있다.

## 하드웨어 메모

- CLI/디버그 UART: Serial1 115200bps(COM3). Modbus: Serial3 RS485.
- ROM 0x40000000~0x4003FFFF(256KB), RAM `.data/.bss` 0x00000000~0x1FFF(8KB). 플래시 옵션 영역은 0x4003BFE0.
- 인터럽트: `startup.as`의 `irq_levN` → `hw/driver/vect.c`의 `irq_vct_tbl`. 인터럽트 키워드는 쓰지 않는다.
- 레지스터는 `bsp/cmsis/srMN103HFD5.h`의 매크로(절대 주소 volatile 접근).
