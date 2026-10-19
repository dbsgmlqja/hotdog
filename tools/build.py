"""HOTDOG 펌웨어 빌드(cc103s/as103s/ld103s/xcv103). DF/BAKERY_DF05.mak와 같은 명령줄을 build/ 폴더에서 실행한다.

build/ 는 DF/ 와 같은 깊이라서 ..\\src\\main.c 같은 상대경로가 DFB5와 똑같고, .x 안의 디버그 정보 경로도 같아진다.

  python tools/build.py              증분 빌드
  python tools/build.py rebuild      전체 다시 빌드
  python tools/build.py clean        build/ 산출물 삭제
  python tools/build.py --dry-run    실행하지 않고 명령만 출력
"""
import argparse
import os
import subprocess
import sys
from pathlib import Path

from hotdog_cfg import ROOT, expand, no_space_path, resolve

SAC_BLOCKED = 4551  # ERROR_... Device Guard/Smart App Control 정책으로 차단


def bs(p):
    return str(p).replace("/", "\\")


def preflight(cc_bin):
    """cmp103s.exe(컴파일러 본체)가 실행 가능한지 확인한다. 서명 없는 exe는 Smart App Control이 막을 수 있다."""
    try:
        subprocess.run([str(cc_bin / "cmp103s.exe")], capture_output=True, timeout=15)
    except OSError as e:
        if getattr(e, "winerror", None) == SAC_BLOCKED:
            print("[오류] cmp103s.exe 실행이 Windows 보안 정책(Smart App Control)에 의해 차단되었습니다(오류 4551).\n"
                  "       서명되지 않은 Nuvoton 컴파일러라서 생기는 문제입니다. docs/VSCODE_SETUP.md의 '빌드가 차단될 때'를 보세요.")
        else:
            print(f"[오류] cmp103s.exe를 실행할 수 없습니다: {e}")
        return False
    return True


class Builder:
    def __init__(self, dfb_root, dry_run, keep_going, hex_path=None):
        self.proj, self.dfb, self.cc = resolve(dfb_root)
        self.dry, self.keep_going = dry_run, keep_going
        self.hex_path = hex_path or self.proj["output"]["hex"]
        self.build = ROOT / self.proj["output"]["build_dir"]
        self.cc_s = Path(no_space_path(self.cc))  # DFB5(.mak)와 같은 경로 표기
        self.bin = self.cc_s / "BIN"
        self.env = dict(os.environ, PATH=str(self.bin) + os.pathsep + os.environ["PATH"])

    def inc(self, key):
        out = []
        for d in self.proj[key]:
            d = expand(d, self.cc_s)
            out.append("-I" + (d if Path(d).is_absolute() else "..\\" + bs(d)))
        return out

    def run(self, label, exe, args):
        shown = " ".join([label, *args])
        print(shown)
        if self.dry:
            return 0
        p = subprocess.run([str(self.bin / exe), *args], cwd=self.build, env=self.env, capture_output=True)
        # cc103s 계열은 출력이 CP932(일본어 Windows 코드페이지)다. -e 옵션으로 메시지는 영어다.
        out = (p.stdout + p.stderr).decode("cp932", errors="replace").strip()
        if out:
            print(out)
        return p.returncode

    def newest_header(self):
        return max((f.stat().st_mtime for f in (ROOT / "src").rglob("*.h")), default=0)

    def obj_name(self, src):
        return Path(src).stem + ".ro"

    def compile_all(self, force):
        hdr = self.newest_header()
        rebuilt, failed = False, False
        for src in self.proj["sources"]:
            obj = self.build / self.obj_name(src)
            s = ROOT / src
            if not force and obj.exists() and obj.stat().st_mtime >= max(s.stat().st_mtime, hdr):
                continue
            out, rel = ".\\" + self.obj_name(src), "..\\" + bs(src)
            if src.endswith(".as"):
                args = [*self.proj["asm_flags"], *self.inc("asm_include_dirs"), "-o", out, rel]
                rc = self.run("AS103S.EXE", "as103s.exe", args)
            else:
                args = [*self.proj["c_flags"], *self.inc("include_dirs"), "-o", out, rel]
                rc = self.run("CC103S.EXE", "cc103s.exe", args)
            rebuilt = True
            if rc != 0:
                failed = True
                print(f"[실패] {src} (종료 코드 {rc})")
                if not self.keep_going:
                    break
        return rebuilt, failed

    def link(self):
        p = self.proj
        lib = expand(p["library"], self.cc_s)
        lines = [f"-o.\\{p['output']['ex']}", " " + " ".join(p["link_flags"]), *p["link_sections"],
                 *[f".\\{o}.ro" for o in p["link_order"]], "-l" + bs(lib)]
        rsp = self.build / "link.rsp"
        if not self.dry:
            rsp.write_text("\n".join(lines) + "\n", encoding="ascii")
        print("LD103S.EXE @link.rsp   (" + " ".join(lines[:5]) + " ... " + lines[-1] + ")")
        if self.dry:
            return 0
        return self.run("LD103S.EXE", "ld103s.exe", ["@link.rsp"])

    def hex(self):
        p = self.proj
        out = (ROOT / self.hex_path).resolve()
        if not self.dry:
            out.parent.mkdir(exist_ok=True)
        rel = bs(os.path.relpath(out, self.build))  # cwd(build/) 기준 상대경로. 기본값이면 ..\OUTPUT\Hotdog.hex
        return self.run("XCV103.EXE", "xcv103.exe", [*p["xcv_flags"], "-o" + rel, ".\\" + p["output"]["ex"]])

    def clean(self):
        if self.build.exists():
            for f in self.build.iterdir():
                if f.suffix in (".ro", ".x", ".map", ".lst", ".rsp"):
                    f.unlink()
        print(f"{self.build.relative_to(ROOT)} 정리 완료")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("action", nargs="?", default="build", choices=["build", "rebuild", "clean"])
    ap.add_argument("--dfb-root")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--keep-going", action="store_true", help="컴파일 오류가 나도 나머지 파일을 계속 컴파일")
    ap.add_argument("--hex", help="hex 출력 경로(저장소 기준). 기본은 project.json의 output.hex(OUTPUT/Hotdog.hex)")
    a = ap.parse_args()

    b = Builder(a.dfb_root, a.dry_run, a.keep_going, a.hex)
    if a.action == "clean":
        b.clean()
        return 0
    if not a.dry_run and not preflight(b.bin):
        return 2
    b.build.mkdir(exist_ok=True)
    if a.action == "rebuild":
        b.clean()
    rebuilt, failed = b.compile_all(force=a.action == "rebuild" or a.dry_run)
    if failed:
        return 1
    ex = b.build / b.proj["output"]["ex"]
    if rebuilt or not ex.exists() or a.dry_run:
        if b.link() != 0:
            return 1
        if b.hex() != 0:
            return 1
    else:
        print("변경 없음(최신 상태)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
