#!/usr/bin/env python3
#
# crashcorg.py
# Playdate crash log analyzer for corgo-engine device builds.
# This script was generated with the help of GenAI, it was reviewed manually but
# may contain errors. Use with caution and verify results against original logs.
# Original information from https://interrupt.memfault.com/blog/cortex-m-hardfault-debug
#
# Copyright (c) 2026 Carlos Camacho. All rights reserved.
#
# Usage:
#   python crashcorg.py                         # use ./crashlog.txt + auto-detected ELF
#   python crashcorg.py path/to/crashlog.txt    # specific log file
#   python crashcorg.py -e build.pd/my.elf      # override ELF path
#   python crashcorg.py --tool-dir /path/to/bin # override ARM toolchain bin directory

import argparse
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Cortex-M Fault Register Bitfield Tables
# ---------------------------------------------------------------------------
# CFSR: Configurable Fault Status Register (0xE000ED28)
# Composed of: MMFSR [7:0] | BFSR [15:8] | UFSR [31:16]
#
# Each entry: (bit_position, flag_name, sub_register_group, description)
CFSR_BITS = [
    # MemManage Fault Status Register (MMFSR) — bits 0-7
    (0,  "IACCVIOL",    "MemManage",  "Instruction access violation (XN region or MPU rule)"),
    (1,  "DACCVIOL",    "MemManage",  "Data access violation"),
    (3,  "MUNSTKERR",   "MemManage",  "Fault on unstacking for exception return"),
    (4,  "MSTKERR",     "MemManage",  "Fault on stacking for exception entry"),
    (5,  "MLSPERR",     "MemManage",  "Fault during FP lazy state preservation"),
    (7,  "MMARVALID",   "MemManage",  "MMFAR holds a valid fault address"),
    # BusFault Status Register (BFSR) — bits 8-15
    (8,  "IBUSERR",     "BusFault",   "Instruction bus error"),
    (9,  "PRECISERR",   "BusFault",   "Precise data bus error (PC points to faulting instruction)"),
    (10, "IMPRECISERR", "BusFault",   "Imprecise data bus error (store was buffered; PC is approximate)"),
    (11, "UNSTKERR",    "BusFault",   "Fault on unstacking for exception return"),
    (12, "STKERR",      "BusFault",   "Fault on stacking for exception entry"),
    (13, "LSPERR",      "BusFault",   "Fault during FP lazy state preservation"),
    (15, "BFARVALID",   "BusFault",   "BFAR holds a valid fault address"),
    # UsageFault Status Register (UFSR) — bits 16-31
    (16, "UNDEFINSTR",  "UsageFault", "Undefined instruction executed"),
    (17, "INVSTATE",    "UsageFault", "Invalid EPSR state (e.g. interworking address missing T-bit)"),
    (18, "INVPC",       "UsageFault", "Invalid EXC_RETURN value on exception exit"),
    (19, "NOCP",        "UsageFault", "Coprocessor access while disabled or absent (check FPU enable)"),
    (24, "UNALIGNED",   "UsageFault", "Unaligned memory access"),
    (25, "DIVBYZERO",   "UsageFault", "Divide by zero"),
]

# HFSR: HardFault Status Register (0xE000ED2C)
HFSR_BITS = [
    (1,  "VECTTBL",  "HardFault", "Vector table read fault during exception processing"),
    (30, "FORCED",   "HardFault", "Escalated from a configurable fault — see CFSR for root cause"),
    (31, "DEBUGEVT", "HardFault", "Debug event (breakpoint/watchpoint) with no debugger enabled"),
]

# ---------------------------------------------------------------------------
# ARM Toolchain Discovery
# ---------------------------------------------------------------------------

# Common Windows install roots for the Arm GNU Toolchain
_WIN_TOOLCHAIN_ROOTS = [
    r"C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi",
    r"C:\Program Files\Arm GNU Toolchain arm-none-eabi",
]


def _find_in_dir(directory, name):
    """Return path to `name` or `name.exe` inside directory, or None."""
    for suffix in ("", ".exe"):
        p = Path(directory) / (name + suffix)
        if p.is_file():
            return str(p)
    return None


def find_arm_tool(name, tool_dir_override=None):
    """
    Locate an arm-none-eabi-* binary.

    Search order:
      1. Explicit --tool-dir override
      2. PATH (via shutil.which)
      3. Known Windows Arm GNU Toolchain install directories (newest version first)
    """
    if tool_dir_override:
        found = _find_in_dir(tool_dir_override, name)
        if found:
            return found

    found = shutil.which(name)
    if found:
        return found

    for root in _WIN_TOOLCHAIN_ROOTS:
        root_path = Path(root)
        if not root_path.is_dir():
            continue
        try:
            versions = sorted(root_path.iterdir(), reverse=True)
        except PermissionError:
            continue
        for version_dir in versions:
            found = _find_in_dir(version_dir / "bin", name)
            if found:
                return found

    return None


# ---------------------------------------------------------------------------
# ELF Discovery
# ---------------------------------------------------------------------------

def find_elf(script_dir):
    """
    Return the best available device ELF path relative to script_dir, or None.
    Prefers the debug build; falls back to the release build.
    """
    candidates = [
        script_dir / "build.pd" / "corgo_engine_DEVICE.elf",
        script_dir / "build.pd.release" / "corgo_engine_RELEASE.elf",
    ]
    for c in candidates:
        if c.is_file():
            return c
    return None


# ---------------------------------------------------------------------------
# Crash Log Parsing
# ---------------------------------------------------------------------------

# Matches:  --- crash at 2026/03/20 06:51:36---
#           --- watchdog reset at 2026/03/20 07:07:56---
_HEADER_RE = re.compile(
    r"^---\s+(.+?)\s+at\s+(\d{4}/\d{2}/\d{2}\s+\d{2}:\d{2}:\d{2})\s*---",
    re.MULTILINE,
)

# Matches register-style  key:hexvalue  pairs (with optional space after colon).
_REG_RE = re.compile(r"(\w+)\s*:\s*([0-9a-fA-F]+)")

# Keys that superficially look like hex registers but are not.
# "build" comes from  build:83737252-3.0.3-...  (value truncated at `-`)
# "allocated" comes from  heap allocated: 148576
_SKIP_REG_KEYS = frozenset({"build", "allocated"})


def _parse_registers(text):
    """Extract all key:hexvalue pairs from a crash block, skipping known non-register fields."""
    result = {}
    for m in _REG_RE.finditer(text):
        key = m.group(1).lower()
        if key in _SKIP_REG_KEYS:
            continue
        try:
            result[key] = int(m.group(2), 16)
        except ValueError:
            pass
    return result


def parse_crash_log(log_text):
    """
    Split the crash log into individual event blocks and parse each one.

    Returns a list of dicts with keys:
      event_type, timestamp, build_id, registers, has_fault_regs,
      heap_bytes, lua_info
    """
    matches = list(_HEADER_RE.finditer(log_text))
    if not matches:
        return []

    events = []
    for i, m in enumerate(matches):
        event_type = m.group(1).strip()
        timestamp  = m.group(2).strip()

        block_start = m.end()
        block_end   = matches[i + 1].start() if i + 1 < len(matches) else len(log_text)
        block_text  = log_text[block_start:block_end]

        regs = _parse_registers(block_text)

        build_match = re.search(r"^build:\s*(\S+)", block_text, re.MULTILINE)
        build_id    = build_match.group(1) if build_match else "unknown"

        heap_match = re.search(r"heap allocated:\s*(\d+)", block_text)
        heap_bytes = int(heap_match.group(1)) if heap_match else None

        lua_match = re.search(
            r"Lua\s+totalbytes=(\d+)\s+GCdebt=(-?\d+)\s+GCestimate=(\d+)\s+stacksize=(\d+)",
            block_text,
        )
        lua_info = None
        if lua_match:
            lua_info = {
                "totalbytes": int(lua_match.group(1)),
                "GCdebt":     int(lua_match.group(2)),
                "GCestimate": int(lua_match.group(3)),
                "stacksize":  int(lua_match.group(4)),
            }

        has_fault_regs = "cfsr" in regs and "hfsr" in regs

        events.append({
            "event_type":     event_type,
            "timestamp":      timestamp,
            "build_id":       build_id,
            "registers":      regs,
            "has_fault_regs": has_fault_regs,
            "heap_bytes":     heap_bytes,
            "lua_info":       lua_info,
        })

    return events


# ---------------------------------------------------------------------------
# Playdate Address Translation
# ---------------------------------------------------------------------------

# Playdate game code is linked at 0x00000000 in the ELF and memory-mapped
# to 0x90000000 at runtime via QSPI NOR flash XIP (STM32H7).
_PLAYDATE_GAME_LOAD_BASE = 0x90000000


def _to_link_time(addr):
    """
    Convert a Playdate runtime game-flash address to the ELF link-time offset.
    Returns (link_time_addr, load_base).  If the address is not in the game
    flash range (0x9000_0000 – 0x9FFF_FFFF), it is returned unchanged with
    load_base=0.
    """
    if 0x90000000 <= addr <= 0x9FFFFFFF:
        return addr - _PLAYDATE_GAME_LOAD_BASE, _PLAYDATE_GAME_LOAD_BASE
    return addr, 0


# ---------------------------------------------------------------------------
# Symbolization via addr2line
# ---------------------------------------------------------------------------

def run_addr2line(addr2line, elf, pc, lr):
    """
    Run arm-none-eabi-addr2line for pc and lr.
    Playdate game-flash addresses (0x9000_xxxx) are first converted to ELF
    link-time offsets by subtracting the load base (0x90000000).
    Returns a dict with resolved strings for 'pc' and 'lr'.
    """
    lt_pc, _ = _to_link_time(pc)
    lt_lr, _ = _to_link_time(lr)
    pc_addr = f"0x{lt_pc:08x}"
    lr_addr = f"0x{lt_lr:08x}"

    def _resolve_one(address):
        result = subprocess.run(
            [addr2line, "-f", "-i", "-p", "-e", elf, address],
            capture_output=True,
            text=True,
            timeout=30,
        )
        output = result.stdout.strip()
        if result.returncode != 0 and result.stderr.strip():
            output += f"\n[addr2line stderr: {result.stderr.strip()}]"
        return output if output else "(no output from addr2line)"

    try:
        return {
            "pc": _resolve_one(pc_addr),
            "lr": _resolve_one(lr_addr),
        }
    except FileNotFoundError:
        return {
            "pc": f"[ERROR] addr2line not found: {addr2line}",
            "lr": f"[ERROR] addr2line not found: {addr2line}",
        }
    except subprocess.TimeoutExpired:
        return {
            "pc": "[ERROR] addr2line timed out after 30s",
            "lr": "[ERROR] addr2line timed out after 30s",
        }


# ---------------------------------------------------------------------------
# Disassembly context via objdump
# ---------------------------------------------------------------------------

def run_disassembly(objdump, elf, pc, context_bytes=80):
    """
    Return (function_name, annotated_disassembly) for a window around `pc`.

    Uses --start-address / --stop-address to keep output small and fast.
    The instruction at pc is marked with '>>>'. Playdate game-flash addresses
    (0x9000_xxxx) are first converted to ELF link-time offsets.
    """
    lt_pc, _ = _to_link_time(pc)
    pc_real  = lt_pc & ~1
    start    = max(0, pc_real - context_bytes)
    stop     = pc_real + context_bytes

    try:
        result = subprocess.run(
            [
                objdump, "-d", "--no-show-raw-insn",
                f"--start-address=0x{start:08x}",
                f"--stop-address=0x{stop:08x}",
                elf,
            ],
            capture_output=True,
            text=True,
            timeout=60,
        )
    except FileNotFoundError:
        return "", f"[ERROR] objdump not found: {objdump}"
    except subprocess.TimeoutExpired:
        return "", "[ERROR] objdump timed out after 60s"

    lines = result.stdout.splitlines()

    # objdump function header:  00001234 <func_name>:
    func_header_re = re.compile(r"^[0-9a-fA-F]+\s+<(.+?)>:")
    # Instruction line:         90003b4a:	push	{r4, lr}
    instr_re = re.compile(r"^\s*([0-9a-fA-F]+):\s+(.+)$")

    func_name = ""
    annotated = []

    for line in lines:
        # Skip ELF file-format header and section-name lines.
        if "file format" in line or line.lstrip().startswith("Disassembly of section"):
            continue

        hdr = func_header_re.match(line)
        if hdr:
            func_name = hdr.group(1)
            annotated.append(line)
            continue

        ins = instr_re.match(line)
        if ins:
            addr   = int(ins.group(1), 16)
            marker = ">>>" if addr == pc_real else "   "
            annotated.append(f"  {marker}  {line.rstrip()}")

    disasm = "\n".join(annotated).strip()
    if not disasm:
        disasm = "(no instructions found in window — address may not map to a code section)"

    return func_name, disasm


# ---------------------------------------------------------------------------
# Fault Register Decoding
# ---------------------------------------------------------------------------

def _decode_bits(value, bit_table):
    """Return list of (group, name, description) for each set bit in value."""
    return [
        (group, name, desc)
        for (bit, name, group, desc) in bit_table
        if value & (1 << bit)
    ]


def format_fault_registers(regs):
    """Return a list of formatted lines describing the decoded fault registers."""
    lines = []
    cfsr  = regs.get("cfsr",  0)
    hfsr  = regs.get("hfsr",  0)
    mmfar = regs.get("mmfar", 0)
    bfar  = regs.get("bfar",  0)

    mmarvalid  = bool(cfsr & (1 << 7))
    bfarvalid  = bool(cfsr & (1 << 15))
    imprecise  = bool(cfsr & (1 << 10))

    # CFSR
    lines.append(f"  CFSR  = 0x{cfsr:08x}")
    cfsr_decoded = _decode_bits(cfsr, CFSR_BITS)
    if cfsr_decoded:
        prev_group = None
        for group, name, desc in cfsr_decoded:
            if group != prev_group:
                lines.append(f"    [{group}]")
                prev_group = group
            lines.append(f"      {name}: {desc}")
    else:
        lines.append("    (no bits set)")

    # HFSR
    lines.append(f"  HFSR  = 0x{hfsr:08x}")
    hfsr_decoded = _decode_bits(hfsr, HFSR_BITS)
    if hfsr_decoded:
        for group, name, desc in hfsr_decoded:
            lines.append(f"      {name}: {desc}")
    else:
        lines.append("    (no bits set)")

    # MMFAR — only meaningful if MMARVALID is set
    if mmarvalid:
        lines.append(f"  MMFAR = 0x{mmfar:08x}  *** valid MemManage fault address ***")
    else:
        lines.append(f"  MMFAR = 0x{mmfar:08x}  (MMARVALID not set — value is not meaningful)")

    # BFAR — only meaningful if BFARVALID is set
    if bfarvalid:
        lines.append(f"  BFAR  = 0x{bfar:08x}  *** valid BusFault address ***")
    else:
        lines.append(f"  BFAR  = 0x{bfar:08x}  (BFARVALID not set — value is not meaningful)")

    # Imprecise bus error advisory
    if imprecise:
        lines.append("")
        lines.append("  NOTE  IMPRECISERR is set: the faulting store was buffered and the")
        lines.append("        exception was taken asynchronously. PC may only indicate the")
        lines.append("        general region of the fault, not the exact instruction.")

    return lines


# ---------------------------------------------------------------------------
# Per-Event Report
# ---------------------------------------------------------------------------

_SEP  = "=" * 72
_RSEP = "-" * 72

# Display order for registers; any extras found in logs are appended after.
_REG_ORDER = ["r0", "r1", "r2", "r3", "r12", "lr", "pc", "psr",
               "cfsr", "hfsr", "mmfar", "bfar", "rcccsr"]


def print_event_report(index, event, elf, addr2line, objdump):
    regs       = event["registers"]
    event_type = event["event_type"]
    timestamp  = event["timestamp"]
    build_id   = event["build_id"]

    print(_SEP)
    print(f"  EVENT #{index}  |  {event_type.upper()}  |  {timestamp}")
    print(f"  build: {build_id}")
    if event["heap_bytes"] is not None:
        print(f"  heap:  {event['heap_bytes']:,} bytes allocated")
    if event["lua_info"]:
        li = event["lua_info"]
        print(
            f"  Lua:   totalbytes={li['totalbytes']}  stacksize={li['stacksize']}"
            f"  GCdebt={li['GCdebt']}  GCestimate={li['GCestimate']}"
        )
    print(_SEP)

    # --- Registers ---
    print()
    print("  Registers:")
    ordered_keys  = [k for k in _REG_ORDER if k in regs]
    extra_keys    = [k for k in sorted(regs) if k not in _REG_ORDER]
    all_keys      = ordered_keys + extra_keys
    cells         = [f"{k:>6}={regs[k]:08x}" for k in all_keys]
    for i in range(0, len(cells), 4):
        print("    " + "    ".join(cells[i:i + 4]))
    print()

    pc = regs.get("pc")
    lr = regs.get("lr")

    if pc is None or lr is None:
        print("  [!] PC and/or LR not found in log — symbolization skipped.")
        print()
        _print_fault_section(event, regs)
        return

    lt_pc, load_base = _to_link_time(pc)
    lt_lr, _          = _to_link_time(lr)

    # --- addr2line ---
    print(_RSEP)
    if load_base:
        print(f"  Symbolization  (runtime PC=0x{pc:08x} -> link-time 0x{lt_pc:08x},  LR=0x{lr:08x} -> 0x{lt_lr:08x})")
    else:
        print(f"  Symbolization  (PC=0x{pc:08x}  LR=0x{lr:08x})")
    print(_RSEP)

    func_from_a2l = ""
    if not elf:
        print("  [!] No device ELF found.")
        print("      Build the device target first, or pass -e <path> to specify the ELF.")
    elif not addr2line:
        print("  [!] arm-none-eabi-addr2line not found.")
        print("      Install the ARM GNU Toolchain or add it to PATH.")
    else:
        a2l_out = run_addr2line(addr2line, elf, pc, lr)
        pc_line = a2l_out.get("pc", "")
        lr_line = a2l_out.get("lr", "")
        print(f"    PC: {pc_line}")
        print(f"    LR: {lr_line}")

        # Prefer PC symbol for disassembly context fallback, then LR.
        for candidate in (pc_line, lr_line):
            if not func_from_a2l and candidate and "??" not in candidate and not candidate.startswith("["):
                func_from_a2l = candidate.split(" at ")[0].strip()

    print()
    print("  NOTE  Only two call frames are recoverable from the Playdate crash log")
    print("        (PC = faulting instruction, LR = caller return address).")
    print("        Add a firmware HardFault handler that dumps SP + stack memory")
    print("        to the log for full call-chain unwinding.")
    print()

    # --- objdump disassembly ---
    lt_display = f"0x{lt_pc & ~1:08x}"
    if load_base:
        lt_display += f"  (runtime 0x{pc & ~1:08x})"
    print(_RSEP)
    print(f"  Disassembly context  (link-time {lt_display})")
    print(_RSEP)
    if not elf:
        print("  (skipped — no ELF)")
    elif not objdump:
        print("  arm-none-eabi-objdump not found — disassembly skipped.")
        print("  Install the ARM GNU Toolchain or add it to PATH.")
    else:
        obj_func, disasm = run_disassembly(objdump, elf, pc)
        if disasm.startswith("(no instructions found in window") and lr is not None:
            caller_addr = (lr - 1) & 0xFFFFFFFF
            lt_caller, caller_base = _to_link_time(caller_addr)
            caller_display = f"0x{lt_caller & ~1:08x}"
            if caller_base:
                caller_display += f"  (runtime 0x{caller_addr & ~1:08x})"
            print("  PC did not map to a code section; showing caller context from LR-1 instead:")
            print(f"  Caller context  (link-time {caller_display})")
            obj_func, disasm = run_disassembly(objdump, elf, caller_addr)

        sym_name = obj_func if obj_func and "??" not in obj_func else func_from_a2l
        if sym_name:
            print(f"  Function: <{sym_name}>")
        for line in disasm.splitlines():
            print(f"  {line}")
    print()

    # --- Fault registers ---
    _print_fault_section(event, regs)


def _print_fault_section(event, regs):
    print(_RSEP)
    print("  Fault Registers")
    print(_RSEP)
    if event["has_fault_regs"]:
        for line in format_fault_registers(regs):
            print(line)
    elif event["event_type"] == "watchdog reset":
        print("  (not present for watchdog reset events)")
    else:
        print("  (cfsr/hfsr not found in this log block)")
    print()


# ---------------------------------------------------------------------------
# Entry Point
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        prog="crashcorg.py",
        description="Parse a Playdate crashlog.txt and symbolize addresses using the device ELF.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python crashcorg.py
  python crashcorg.py path/to/crashlog.txt
  python crashcorg.py -e build.pd/corgo_engine_DEVICE.elf
  python crashcorg.py --tool-dir "C:\\Program Files (x86)\\Arm GNU Toolchain arm-none-eabi\\12.2 mpacbti-rel1\\bin"
        """,
    )
    parser.add_argument(
        "log",
        nargs="?",
        default="crashlog.txt",
        metavar="LOG",
        help="Crash log file to parse (default: crashlog.txt)",
    )
    parser.add_argument(
        "-e", "--elf",
        default=None,
        metavar="ELF",
        help="Path to device ELF (default: auto-detected from build.pd/)",
    )
    parser.add_argument(
        "--tool-dir",
        default=None,
        metavar="DIR",
        help="Directory containing arm-none-eabi-* binaries (overrides PATH and auto-detect)",
    )
    args = parser.parse_args()

    script_dir = Path(__file__).parent.resolve()

    # Resolve log path (cwd first, then script_dir)
    log_path = Path(args.log)
    if not log_path.is_absolute() and not log_path.exists():
        alt = script_dir / log_path
        if alt.exists():
            log_path = alt
    if not log_path.exists():
        print(f"ERROR: crash log not found: {log_path}", file=sys.stderr)
        return 1

    log_text = log_path.read_text(encoding="utf-8", errors="replace")

    # Resolve ELF
    if args.elf:
        elf_path = Path(args.elf)
        if not elf_path.is_absolute():
            elf_path = Path.cwd() / elf_path
        elf = str(elf_path) if elf_path.is_file() else None
        if not elf:
            print(f"WARNING: specified ELF not found: {args.elf}", file=sys.stderr)
    else:
        found = find_elf(script_dir)
        elf = str(found) if found else None

    # Resolve tools
    addr2line = find_arm_tool("arm-none-eabi-addr2line", args.tool_dir)
    objdump   = find_arm_tool("arm-none-eabi-objdump",   args.tool_dir)

    # Print resolution summary
    print(f"Log:      {log_path}")
    print(f"ELF:      {elf or '(not found)'}")
    print(f"addr2line:{(' ' + addr2line) if addr2line else ' (not found)'}")
    print(f"objdump:  {objdump or '(not found)'}")
    print()

    if not elf:
        print(
            "WARNING: No device ELF found. Build the device target first:\n"
            "         cmake --build --preset build-pd-debug\n"
            "         or pass -e <path> to specify the ELF manually.",
            file=sys.stderr,
        )
    if not addr2line:
        print(
            "WARNING: arm-none-eabi-addr2line not found. Symbolization will be skipped.\n"
            "         Install the ARM GNU Toolchain and add its bin/ to PATH,\n"
            "         or pass --tool-dir <dir>.",
            file=sys.stderr,
        )

    # Parse
    events = parse_crash_log(log_text)
    if not events:
        print("No crash events found in log file.")
        return 0

    print(f"Found {len(events)} event(s) in {log_path.name}")
    print()

    for i, event in enumerate(events, start=1):
        print_event_report(i, event, elf, addr2line, objdump)

    print(_SEP)
    return 0


if __name__ == "__main__":
    sys.exit(main())
