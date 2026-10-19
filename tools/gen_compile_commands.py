"""build/compile_commands.json 생성. clangd, clang-tidy, cppcheck가 쓴다(실제 컴파일은 cc103s가 한다)."""
import argparse
import json

from hotdog_cfg import ROOT, expand, resolve


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--dfb-root", help="DFBuilder5 설치 경로(기본: 환경변수 DFB_ROOT 또는 project.json)")
    args = ap.parse_args()
    proj, _, cc = resolve(args.dfb_root)

    cc_inc = str(cc / "INCLUDE")
    inc_flags = []
    for d in proj["include_dirs"]:
        d = expand(d, cc)
        if d == cc_inc:
            inc_flags += ["-isystem", d.replace("\\", "/")]
        else:
            inc_flags.append("-I" + (ROOT / d).as_posix())

    entries = []
    for src in proj["sources"]:
        if not src.endswith(".c"):
            continue
        f = (ROOT / src).as_posix()
        entries.append({
            "directory": ROOT.as_posix(),
            "file": f,
            "arguments": ["clang", *proj["clang"]["flags"], *inc_flags, "-c", f],
        })

    out = ROOT / proj["output"]["build_dir"] / "compile_commands.json"
    out.parent.mkdir(exist_ok=True)
    out.write_text(json.dumps(entries, indent=1), encoding="utf-8")
    print(f"{out.relative_to(ROOT)}: {len(entries)}개 파일")


if __name__ == "__main__":
    main()
