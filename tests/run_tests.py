"""호스트(PC) 유닛테스트 실행기. 실제 src/ 코드를 clang으로 PC용으로 컴파일해 돌린다(보드/DFBuilder5 불필요).

  python tests/run_tests.py            전부 실행
  python tests/run_tests.py qbuffer    이름에 'qbuffer'가 든 테스트만

새 테스트: tests/host/test_<이름>.c 를 만들고 아래 TESTS 에 함께 링크할 소스를 적는다.
"""
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from hotdog_cfg import load_project  # noqa: E402

# 테스트 이름 -> 함께 컴파일할 소스. (modbus는 테스트 파일이 modbus.c를 직접 #include 한다)
TESTS = {
    "test_qbuffer": ["src/common/qbuffer.c"],
    "test_pid": ["src/hw/driver/Pid.c"],
    "test_modbus_crc": ["tests/host/stubs.c"],
}


def main():
    pattern = sys.argv[1] if len(sys.argv) > 1 else ""
    proj = load_project()
    inc = [f"-I{ROOT / d}" for d in proj["include_dirs"] if not d.startswith("${")]
    out_dir = ROOT / proj["output"]["build_dir"] / "tests"
    out_dir.mkdir(parents=True, exist_ok=True)

    failed = []
    for name, srcs in TESTS.items():
        if pattern not in name:
            continue
        exe = out_dir / f"{name}.exe"
        # -g 는 일부러 뺀다: 이 PC에서 -g로 만든 실행 파일이 Smart App Control에 막히는 경우가 있었다(WinError 4551).
        cmd = ["clang", "-std=gnu99", "-w", f"-I{ROOT / 'tests/host'}", *inc,
               str(ROOT / "tests/host" / f"{name}.c"), *[str(ROOT / s) for s in srcs], "-o", str(exe)]
        print(f"=== {name}")
        cp = subprocess.run(cmd, capture_output=True, text=True)
        if cp.returncode != 0:
            print("  빌드 실패\n" + cp.stdout + cp.stderr)
            failed.append(name)
            continue
        try:
            rp = subprocess.run([str(exe)], capture_output=True, text=True)
        except OSError as e:
            print(f"  실행 실패: {e}\n  (WinError 4551이면 Windows 보안 정책이 이 실행 파일을 막은 것입니다. 다시 빌드하면 판정이 달라지기도 합니다.)")
            failed.append(name)
            continue
        print(rp.stdout.rstrip())
        if rp.returncode != 0:
            failed.append(name)

    print("\n" + ("전체 통과" if not failed else "실패: " + ", ".join(failed)))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
