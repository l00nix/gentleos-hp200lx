#!/usr/bin/env python3

import glob
import os

SOURCE_DIRS = ["apps", "data", "kernel", "lib", "gui"]

EXCLUDED_SOURCES = [
]

MAKEFILE_TPL = """
CC      = tcc
AS      = vendor\\nasm301\\nasm
CFLAGS  = -mt -u- -g1 -c -Iinclude
ASFLAGS = -f obj

OBJS = &
<OBJS>

INCLUDES = &
<INCLUDES>

all: build\\fd1440.img .SYMBOLIC
    @echo All done!

boot: all .SYMBOLIC
    boot build\\fd1440.img

build\\kernel.com: $(OBJS)
	tlink @misc\\tlink.rsp

build\\boot.bin: boot\\boot.s
    $(AS) -o build\\boot.bin boot\\boot.s

build\\mkdisk.exe: misc\\mkdisk.c
    $(CC) -nbuild misc\\mkdisk.c

build\\fd1440.img: build\\kernel.com build\\boot.bin build\\mkdisk.exe
    build\\mkdisk

clean: .SYMBOLIC
	-del build\\data\\*.obj
	-del build\\gui\\*.obj
	-del build\\lib\\*.obj
	-del build\\kernel\\*.obj
	-del build\\*.*

<OBJECT_RULES>
"""

C_RULE_TPL = """
<OBJ_PATH>: <SRC> $(INCLUDES)
	$(CC) $(CFLAGS) -n<OBJ_DIR> <SRC>
"""

S_RULE_TPL = """
<OBJ_PATH>: <SRC>
	$(AS) $(ASFLAGS) -o <OBJ_PATH> <SRC>
"""


def collect_sources():
    sources = []

    for d in SOURCE_DIRS:
        sources.extend(glob.glob(f"{d}/*.c"))
        sources.extend(glob.glob(f"{d}/*.s"))

    sources = [s for s in sources if not s in EXCLUDED_SOURCES]
    sources.sort()
    sources.remove("kernel/start.s")
    sources = ["kernel/start.s"] + sources

    return sources


def collect_includes():
    includes = sorted(glob.glob("include/*.h"))
    includes = [h for h in includes if not h.endswith(".sample.h")]

    return includes


def path_to_dos(path):
    return path.replace("/", "\\")


def make_object_rule(src):
    base, ext = os.path.splitext(src)
    tpl = C_RULE_TPL if ext == ".c" else S_RULE_TPL

    return tpl.strip("\n") \
        .replace("<SRC>", path_to_dos(src)) \
        .replace("<OBJ_PATH>", f"build\\{path_to_dos(base)}.obj") \
        .replace("<OBJ_DIR>", f"build\\{path_to_dos(os.path.dirname(src))}")


def generate_makefile(sources, includes):
    objs = "\n".join(
        f"\tbuild\\{path_to_dos(os.path.splitext(src)[0])}.obj &" for src in sources
    )

    incs = "\n".join(
        f"\t{path_to_dos(h)} &" for h in includes
    )

    rules = "\n\n".join(make_object_rule(src) for src in sources)

    content = MAKEFILE_TPL.lstrip("\n") \
        .replace("<OBJS>", objs) \
        .replace("<INCLUDES>", incs) \
        .replace("<OBJECT_RULES>", rules)

    with open("Makefile", "w", newline="") as f:
        f.write(content.replace("\n", "\r\n"))


def generate_tlink_rsp(sources):
    lines = []

    lines.append("/s /t /l +")

    for i, src in enumerate(sources):
        base = os.path.splitext(src)[0]
        obj_path = f"build\\{path_to_dos(base)}.obj"
        if i < len(sources) - 1:
            lines.append(f"\t{obj_path}+")
        else:
            lines.append(f"\t{obj_path},build\\kernel.com,build\\kernel.map")

    lines.append("")

    with open("misc/tlink.rsp", "w", newline="") as f:
        f.write("\r\n".join(lines))


def main():
    sources = collect_sources()
    includes = collect_includes()
    generate_makefile(sources, includes)
    generate_tlink_rsp(sources)


if __name__ == "__main__":
    main()
