"""DF/BAKERY_DF05.mak(DFBuilder5가 만든 빌드 정의)와 tools/project.json이 어긋났는지 비교한다.
DFB5에서 소스를 추가/삭제했는데 project.json을 갱신하지 않았을 때 알려 준다. 어긋남이 있으면 종료 코드 1.
"""
import re
import sys

from hotdog_cfg import ROOT, load_project

MAK = ROOT / "DF" / "BAKERY_DF05.mak"


def norm(p):
    return p.replace("\\", "/").removeprefix("../").removeprefix("-I../").strip()


def block(text, start, end):
    m = re.search(start + r"(.*?)" + end, text, re.S | re.M)
    return m.group(1) if m else ""


def main():
    if not MAK.exists():
        print(f"{MAK.relative_to(ROOT)} 가 없습니다(DFB5에서 한 번 빌드하면 생성됩니다).")
        return 1
    mak = MAK.read_text(encoding="cp949", errors="replace")
    proj = load_project()
    diffs = []

    def cmp(name, mak_v, proj_v):
        if mak_v != proj_v:
            only_mak = [x for x in mak_v if x not in proj_v]
            only_proj = [x for x in proj_v if x not in mak_v]
            diffs.append(f"[{name}] .mak에만: {only_mak}  project.json에만: {only_proj}"
                         + ("" if only_mak or only_proj else "  (항목은 같고 순서가 다름)"))

    src = [norm(m) for m in re.findall(r"^\s*(\.\.\\src\\\S+?)\s*\\?\s*$", block(mak, r"SOURCEFILE =\\", r"OBJFILES"), re.M)]
    cmp("sources", src, proj["sources"])

    link = re.findall(r"\$\(OBJ_DIR\)(\w+)\$\(OBJ_EXT\)\s*$", block(mak, r"-T\.rodata_FlashOption=\w+", r"\$\(LIBFILES\)"), re.M)
    cmp("link_order", link, proj["link_order"])

    def flags(name):
        m = re.search(rf"^{name}\s*=\s*(.*)$", mak, re.M)
        return m.group(1).split() if m else []

    cmp("c_flags", flags("CFLAGS"), proj["c_flags"])
    cmp("asm_flags", flags("AFLAGS"), proj["asm_flags"])
    cmp("link_flags", flags("LFLAGS"), proj["link_flags"])

    sections = re.findall(r"^\s*(-T\S+)", block(mak, r"\$\(LINK\) @&&\|", r"\$\(OBJ_DIR\)startup"), re.M)
    cmp("link_sections", sections, proj["link_sections"])

    # include 경로: CC 헤더 폴더는 PC마다 달라 자리표시자로 비교한다.
    def incs(name, nxt):
        items = re.findall(r"-I(\S+?)\s*\\?\s*$", block(mak, rf"^{name} =\\", nxt), re.M)
        return [("${CC_INCLUDE}" if i.upper().endswith("\\INCLUDE") and "BuiltInCC" in i else norm(i)) for i in items]

    cmp("include_dirs", incs("CINC", r"^AINC"), proj["include_dirs"])
    cmp("asm_include_dirs", incs("AINC", r"^LIBFILES"), proj["asm_include_dirs"])

    if diffs:
        print("DFB5(.mak)와 tools/project.json이 다릅니다:")
        print("\n".join("  " + d for d in diffs))
        return 1
    print(f"일치: 소스 {len(src)}개, 링크 {len(link)}개, 플래그/경로 동일")
    return 0


if __name__ == "__main__":
    sys.exit(main())
