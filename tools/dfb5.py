"""'빌드 → DFB5 Make → 보드 다운로드 → 실행'을 한 번에 한다. 보드를 조작하므로 실행 전에 반드시 확인을 받는다.

  python tools/dfb5.py run               전체(확인 후 보드 조작)
  python tools/dfb5.py run --no-board    보드를 건드리지 않고 빌드/Make와 매크로 생성까지만
  python tools/dfb5.py run --no-go       다운로드만 하고 실행(G)은 하지 않음
  python tools/dfb5.py run --confirm yes 확인 질문 생략(VSCode 태스크가 선택창에서 받은 답을 넘길 때 쓴다)

동작 원리: DFB5는 `DFB5.exe <그룹/프로젝트> /C"<명령>"` 으로 기동 직후 명령을 실행할 수 있다(매뉴얼 StartupOptions).
build/dfb5_run.mcr 매크로에 다음을 적어 넘긴다.
    >로그파일          명령창 출력을 파일에 기록
    DEBUGMODE /LOAD    타깃을 열고 프로젝트의 DF/BAKERY_DF05.x 를 보드 Flash에 기록
    G                  실행
    >                  로그 닫기
DFB5가 이미 열려 있으면 같은 보드를 동시에 쓸 수 없으므로 보드 단계만 건너뛰고 안내한다(Make까지는 한다).
"""
import argparse
import os
import subprocess
import sys
import time
from pathlib import Path

from hotdog_cfg import ROOT, load_project, no_space_path, resolve

EXIT_NOT_DOWNLOADED = 3  # 빌드/Make는 끝났지만 보드에는 올리지 않았다


def dfb5_running():
    out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq DFB5.exe", "/NH"], capture_output=True, errors="replace").stdout
    return "DFB5.exe" in (out if isinstance(out, str) else out.decode("cp949", "replace"))


def banner(text):
    print(f"\n=== {text}", flush=True)


def run_vscode_build(dfb):
    banner("1/4 VSCode 빌드(오류 확인용)")
    cmd = [sys.executable, str(ROOT / "tools" / "build.py"), "--dfb-root", str(dfb), "--hex", "build/Hotdog_vscode.hex"]
    return subprocess.run(cmd).returncode == 0


def run_dfmake(proj, dfb, cc):
    """DFB5의 Make와 같은 DFMake를 DF/ 에서 실행해 DF/BAKERY_DF05.x 와 OUTPUT/Hotdog.hex 를 갱신한다."""
    banner("2/4 DFB5 Make(DFMake) → DF/" + proj["output"]["ex"])
    mak = ROOT / proj["dfb5"]["makefile"]
    if not mak.exists():
        print(f"{mak.relative_to(ROOT)} 가 없습니다. DFB5에서 한 번 Make 하면 생성됩니다.")
        return False
    dfp5 = ROOT / proj["dfb5"]["project"]
    if dfp5.exists() and dfp5.stat().st_mtime > mak.stat().st_mtime + 1:
        print("[주의] .dfp5가 .mak보다 새롭습니다. DFB5에서 프로젝트를 바꿨다면 DFB5에서 Make를 한 번 눌러 .mak을 갱신하세요.")
    env = dict(os.environ, PATH=no_space_path(cc / "BIN") + os.pathsep + os.environ["PATH"])
    # -F 는 반드시 첫 인자여야 한다(앞에 다른 옵션이 오면 DFMake가 조용히 종료한다).
    cmdline = f'"{dfb / "Bin" / "DFMake.exe"}" -F{mak.name} {proj["output"]["ex"]}'
    p = subprocess.run(cmdline, cwd=mak.parent, env=env, capture_output=True)
    out = (p.stdout + p.stderr).decode("cp932", errors="replace").strip()
    print(out)
    return p.returncode == 0 and "Make failed" not in out


def confirm(args):
    if args.confirm == "yes":
        return True
    if args.confirm == "no" or not sys.stdin.isatty():
        return False
    print("\n보드에 다운로드하고 실행합니다. 히터/모터/SSR 부하가 연결되어 있으면 동작할 수 있습니다.")
    return input("계속할까요? (y/N): ").strip().lower() in ("y", "yes", "예")


def write_macro(proj, no_go):
    build = ROOT / proj["output"]["build_dir"]
    build.mkdir(exist_ok=True)
    log, mcr = build / "dfb5_session.log", build / "dfb5_run.mcr"
    if log.exists():
        log.unlink()
    lines = [f">{log}", "DEBUGMODE /LOAD"] + ([] if no_go else ["G"]) + [">"]
    mcr.write_bytes(("\r\n".join(lines) + "\r\n").encode("ascii"))  # write_text 는 \n 을 \r\n 으로 또 바꿔서 쓰지 않는다
    return mcr, log


def wait_for_load(log, timeout, proc):
    """로그에서 다운로드 완료를 기다린다. (완료 여부, 로그 내용)을 돌려준다."""
    end = time.time() + timeout
    text = ""
    while time.time() < end:
        if log.exists():
            text = log.read_bytes().decode("cp949", errors="replace")
            if "Load completed" in text:
                time.sleep(3)  # G 명령과 로그 닫기까지 여유
                return True, log.read_bytes().decode("cp949", errors="replace")
            if any(k in text for k in ("rror", "Cannot", "cannot", "failed", "Failed")):
                return False, text
        if proc.poll() is not None and not log.exists():
            return False, "DFB5가 로그를 남기기 전에 종료되었습니다."
        time.sleep(1)
    return False, text


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("action", choices=["run"])
    ap.add_argument("--dfb-root")
    ap.add_argument("--no-board", action="store_true", help="보드를 조작하지 않는다(시험용)")
    ap.add_argument("--no-go", action="store_true", help="다운로드만 하고 실행하지 않는다")
    ap.add_argument("--no-build", action="store_true", help="VSCode 빌드 단계를 건너뛴다")
    ap.add_argument("--confirm", choices=["yes", "no"], help="확인 질문에 대한 답(없으면 터미널에서 묻는다)")
    args = ap.parse_args()
    sys.stdout.reconfigure(errors="replace")

    proj, dfb, cc = resolve(args.dfb_root)
    exe = dfb / "Bin" / "DFB5.exe"
    if not exe.exists():
        sys.exit(f"DFB5.exe를 찾지 못했습니다: {exe}")

    if not args.no_build and not run_vscode_build(dfb):
        print("\n[중단] VSCode 빌드에 실패했습니다. 오류를 고친 뒤 다시 실행하세요.")
        return 1
    if not run_dfmake(proj, dfb, cc):
        print("\n[중단] DFB5 Make에 실패했습니다. 보드에는 아무것도 하지 않았습니다.")
        return 1

    banner("3/4 보드 조작 확인")
    if args.no_board:
        print("--no-board: 보드를 조작하지 않습니다. 매크로만 만들어 둡니다.")
        mcr, log = write_macro(proj, args.no_go)
        print(f"매크로: {mcr.relative_to(ROOT)}\n" + mcr.read_bytes().decode("ascii").replace("\r\n", "\n"))
        return 0
    if dfb5_running():
        print("DFB5가 이미 실행 중이라 보드 다운로드는 하지 않았습니다(같은 보드를 동시에 쓸 수 없음).\n"
              "Make는 끝났으니 DFB5에서 Run 을 누르거나, DFB5를 종료한 뒤 이 태스크를 다시 실행하세요.")
        return EXIT_NOT_DOWNLOADED
    if not confirm(args):
        print("취소했습니다. 보드에는 아무것도 하지 않았습니다.")
        return EXIT_NOT_DOWNLOADED

    banner("4/4 DFB5 기동 → 다운로드" + ("" if args.no_go else " → 실행"))
    mcr, log = write_macro(proj, args.no_go)
    group, prj = ROOT / proj["dfb5"]["group"], ROOT / proj["dfb5"]["project"]
    target = group if group.exists() else prj
    cmdline = f'"{exe}" "{target}" /C"<{mcr}"'
    print(f"DFB5 열기: {target.relative_to(ROOT)}   매크로: {mcr.relative_to(ROOT)}")
    proc = subprocess.Popen(cmdline, cwd=target.parent,
                            creationflags=subprocess.DETACHED_PROCESS | subprocess.CREATE_NEW_PROCESS_GROUP, close_fds=True)
    ok, text = wait_for_load(log, proj["dfb5"]["load_timeout_sec"], proc)
    print("\n--- DFB5 명령창 기록 ---\n" + text.strip() + "\n------------------------")
    if ok:
        print("다운로드 완료를 확인했습니다." + ("" if args.no_go else " 실행(G) 명령을 보냈습니다. 상태는 DFB5 창 아래쪽 막대(Target executing)에서 확인하세요."))
        return 0
    print("다운로드 완료를 확인하지 못했습니다. DFB5 창에 대화상자나 오류가 떠 있는지 확인하세요.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
