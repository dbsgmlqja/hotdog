"""링커 맵(.map)에서 ROM/RAM 사용량을 요약하고 이전 빌드와 비교한다.

  python tools/memreport.py              build/BAKERY_DF05.map (없으면 DF/BAKERY_DF05.map)
  python tools/memreport.py --save       결과를 build/memreport.json 이력에 추가(다음 실행부터 변화량 표시)
  python tools/memreport.py --top 20     파일별 상위 20개

계산식은 DFB5가 DF/*.map.log에 남기는 값과 같다:
  ROM = .text + .rodata + .MN.romdata + .rodata_FlashOption (0x40000000 이상, 쓰기 불가 섹션)
  RAM = .MN.data + .bss (0x40000000 미만, 쓰기 가능 섹션)
"""
import argparse
import json
import re
import sys
import time
from pathlib import Path

from hotdog_cfg import ROOT, load_project

ROM_BASE, ROM_SIZE = 0x40000000, 0x40000   # 내장 ROM 0x40000000~0x4003FFFF (DF/pice103.env)
RAM_BASE, RAM_SIZE = 0x00000000, 0x2000    # .data/.bss 가 놓이는 RAM 0x00000000~0x00001FFF


def classify(addr, attr):
    if attr == "n":                      # 디버그 정보는 제외
        return None
    if addr >= ROM_BASE:
        return "rom"
    return "ram" if "w" in attr else None


def parse(path):
    """(sections, files) 를 돌려준다. sections: {이름: (주소, 길이, 속성)}, files: {파일: {rom, ram}}."""
    sections, files, mode, cur = {}, {}, None, None
    for line in Path(path).read_text(encoding="cp949", errors="replace").splitlines():
        if line.startswith("#SECTION"):
            mode = "sec"
        elif line.startswith("#FILE"):
            mode = "file"
        elif re.match(r"#[A-Z]", line):
            mode = None
        elif not line.strip() or line.startswith((";", "#")):
            continue
        else:
            p = line.split()
            if mode == "sec" and len(p) >= 4:
                sections[p[3]] = (int(p[0], 16), int(p[1], 16), p[2].strip('"'))
            elif mode == "file" and len(p) >= 5:
                if p[2] == "FILE":
                    cur = files.setdefault(p[4], {"rom": 0, "ram": 0})
                elif cur is not None:
                    kind = classify(int(p[0], 16), p[2].strip('"'))
                    if kind:
                        cur[kind] += int(p[1], 16)
    return sections, files


def summarize(sections):
    tot = {"rom": 0, "ram": 0}
    detail = {}
    for name, (addr, length, attr) in sections.items():
        kind = classify(addr, attr)
        if kind and length:
            tot[kind] += length
            detail[name] = length
    return tot, detail


def delta(now, before):
    d = now - before
    return f"{d:+,}" if d else "±0"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("map", nargs="?")
    ap.add_argument("--save", action="store_true")
    ap.add_argument("--top", type=int, default=10)
    a = ap.parse_args()

    proj = load_project()
    build = ROOT / proj["output"]["build_dir"]
    path = Path(a.map) if a.map else next((p for p in (build / proj["output"]["map"], ROOT / "DF" / proj["output"]["map"]) if p.exists()), None)
    if path is None:
        sys.exit("맵 파일이 없습니다. 먼저 빌드하세요.")

    sections, files = parse(path)
    tot, detail = summarize(sections)
    hist_path = build / "memreport.json"
    hist = json.loads(hist_path.read_text(encoding="utf-8")) if hist_path.exists() else []
    prev = hist[-1] if hist else None

    print(f"맵: {path.relative_to(ROOT) if path.is_relative_to(ROOT) else path}")
    for kind, label, size in (("rom", "ROM", ROM_SIZE), ("ram", "RAM", RAM_SIZE)):
        s = f"{label} {tot[kind]:>8,} / {size:>8,} B ({tot[kind] * 100 / size:5.1f}%)"
        if prev:
            s += f"   이전 대비 {delta(tot[kind], prev[kind])}"
        print(s)
    print("  " + "  ".join(f"{n} {v:,}" for n, v in detail.items()))

    rows = sorted(files.items(), key=lambda kv: kv[1]["rom"] + kv[1]["ram"], reverse=True)[:a.top]
    print(f"\n파일별 상위 {a.top} (ROM / RAM)")
    for name, v in rows:
        s = f"  {name:<18}{v['rom']:>8,}{v['ram']:>8,}"
        if prev and name in prev["files"]:
            pv = prev["files"][name]
            if (v["rom"], v["ram"]) != (pv["rom"], pv["ram"]):
                s += f"   ({delta(v['rom'], pv['rom'])} / {delta(v['ram'], pv['ram'])})"
        print(s)

    if prev:
        changed = [n for n in files if n not in prev["files"] or files[n] != prev["files"][n]]
        gone = [n for n in prev["files"] if n not in files]
        if changed or gone:
            print(f"\n변경된 파일: {', '.join(changed) or '-'}" + (f"   사라진 파일: {', '.join(gone)}" if gone else ""))

    if a.save:
        build.mkdir(exist_ok=True)
        hist.append({"time": time.strftime("%Y-%m-%d %H:%M:%S"), "map": str(path), **tot, "files": files})
        hist_path.write_text(json.dumps(hist[-50:], indent=1), encoding="utf-8")
        print(f"\n이력 저장: {hist_path.relative_to(ROOT)} ({len(hist[-50:])}건)")


if __name__ == "__main__":
    main()
