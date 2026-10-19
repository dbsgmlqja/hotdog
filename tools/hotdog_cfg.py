"""project.json 로딩과 DFBuilder5 경로 해석. build.py, gen_compile_commands.py 등이 공유한다."""
import ctypes
import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def load_project():
    with open(ROOT / "tools" / "project.json", encoding="utf-8") as f:
        return json.load(f)


def short_path(p):
    buf = ctypes.create_unicode_buffer(1024)
    n = ctypes.windll.kernel32.GetShortPathNameW(str(p), buf, 1024)
    return buf.value if n else str(p)


def no_space_path(p):
    """공백이 든 폴더 이름만 8.3으로 줄인다. DFB5의 CCInstallNSPath와 같은 형태(C:\\PROGRA~2\\Nuvoton\\DFBuilder5\\...)다.
    .x 디버그 정보에 헤더 경로가 문자열로 남을 수 있어서 DFB5와 글자까지 맞춘다."""
    p = Path(p)
    cur = Path(p.anchor)
    for part in p.parts[1:]:
        cur = cur / part
        if " " in part:
            cur = cur.parent / Path(short_path(cur)).name
    return str(cur)


def resolve(dfb_root=None):
    """(project, dfb_root, cc_root)를 돌려준다. 우선순위: 인자 > 환경변수 DFB_ROOT > project.json 기본값."""
    proj = load_project()
    dfb = Path(dfb_root or os.environ.get("DFB_ROOT") or proj["compiler"]["dfb_root_default"])
    cc = dfb / "BuiltInCC" / proj["compiler"]["series"] / proj["compiler"]["version"]
    if not (cc / "BIN" / "cc103s.exe").exists():
        raise SystemExit(f"cc103s.exe를 찾지 못했습니다: {cc}\n--dfb-root 또는 환경변수 DFB_ROOT로 DFBuilder5 설치 경로를 지정하세요.")
    return proj, dfb, cc


def expand(s, cc_root):
    """project.json의 ${CC_ROOT}, ${CC_INCLUDE} 치환."""
    return s.replace("${CC_INCLUDE}", str(cc_root / "INCLUDE")).replace("${CC_ROOT}", str(cc_root))
