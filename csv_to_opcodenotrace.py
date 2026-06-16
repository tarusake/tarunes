#!/usr/bin/env python3
import argparse
import csv
import re
import sys
from pathlib import Path
from typing import TextIO


DEFAULT_INPUT = Path("tarunes_tangnano20k/impl/wave/cpu_trace_core0.csv")
DEFAULT_CPU = Path("src/cpu.veryl")
DEFAULT_OUTPUT = Path("cpu_opcode_trace.log")

OPCODE_RE = re.compile(
    r"opcode:\s*8'h([0-9A-Fa-f]{2}).*?inst_kind:\s*inst_t::([A-Za-z0-9_]+)"
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Extract valid PC/opcode rows from a Gowin CPU trace CSV."
    )
    parser.add_argument(
        "input",
        nargs="?",
        type=Path,
        default=DEFAULT_INPUT,
        help=f"input CSV file (default: {DEFAULT_INPUT})",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT,
        help=f"output file, or '-' for stdout (default: {DEFAULT_OUTPUT})",
    )
    parser.add_argument(
        "--cpu",
        type=Path,
        default=DEFAULT_CPU,
        help=f"Veryl CPU source used for opcode mnemonics (default: {DEFAULT_CPU})",
    )
    parser.add_argument(
        "--pc-column",
        default="core/cpu_inst/reg_pc[15:0]",
        help="CSV column containing the PC value",
    )
    parser.add_argument(
        "--opcode-column",
        default="core/cpu_inst/decode_entry.opcode[7:0]",
        help="CSV column containing the opcode value",
    )
    parser.add_argument(
        "--valid-column",
        default="logic_analizer[15]",
        help="CSV column used as valid flag",
    )
    parser.add_argument(
        "--no-header",
        action="store_true",
        help="do not write the 'PC OPCODE MNEMONIC' header",
    )
    return parser.parse_args()


def load_mnemonics(cpu_path: Path) -> dict[str, str]:
    mnemonics: dict[str, str] = {}
    text = cpu_path.read_text(encoding="utf-8")
    for match in OPCODE_RE.finditer(text):
        opcode, mnemonic = match.groups()
        mnemonics[opcode.upper()] = mnemonic.upper()
    return mnemonics


def open_wave_csv(path: Path) -> tuple[list[str], csv.reader]:
    input_file = path.open("r", encoding="utf-8", errors="replace", newline="")

    for line in input_file:
        if line.strip() == "Data:":
            break
    else:
        input_file.close()
        raise ValueError("Data: section was not found")

    reader = csv.reader(input_file, skipinitialspace=True)
    try:
        headers = next(reader)
    except StopIteration as exc:
        input_file.close()
        raise ValueError("CSV header was not found after Data:") from exc

    return [header.strip() for header in headers], reader


def column_index(headers: list[str], name: str) -> int:
    try:
        return headers.index(name)
    except ValueError as exc:
        available = "\n  ".join(header for header in headers if header)
        raise ValueError(f"column not found: {name}\navailable columns:\n  {available}") from exc


def normalize_hex(value: str, width: int) -> str:
    value = value.strip().upper()
    if not value or "X" in value or "Z" in value:
        raise ValueError(f"not a known hex value: {value!r}")
    if value.startswith("0X"):
        value = value[2:]
    parsed = int(value, 16)
    return f"{parsed:0{width}X}"


def write_opcodenotrace(
    input_path: Path,
    output: TextIO,
    mnemonics: dict[str, str],
    pc_column: str,
    opcode_column: str,
    valid_column: str,
    write_header: bool,
) -> int:
    headers, reader = open_wave_csv(input_path)
    pc_index = column_index(headers, pc_column)
    opcode_index = column_index(headers, opcode_column)
    valid_index = column_index(headers, valid_column)

    if write_header:
        output.write("PC OPCODE MNEMONIC\n")

    count = 0
    for row in reader:
        if not row or valid_index >= len(row):
            continue

        valid = row[valid_index].strip()
        if valid != "1":
            continue

        try:
            pc = normalize_hex(row[pc_index], 4)
            opcode = normalize_hex(row[opcode_index], 2)
        except (IndexError, ValueError):
            continue

        mnemonic = mnemonics.get(opcode, "???")
        output.write(f"{pc} {opcode} {mnemonic}\n")
        count += 1

    return count


def main() -> int:
    args = parse_args()

    try:
        mnemonics = load_mnemonics(args.cpu)
        if args.output == Path("-"):
            count = write_opcodenotrace(
                args.input,
                sys.stdout,
                mnemonics,
                args.pc_column,
                args.opcode_column,
                args.valid_column,
                not args.no_header,
            )
        else:
            with args.output.open("w", encoding="utf-8", newline="") as output:
                count = write_opcodenotrace(
                    args.input,
                    output,
                    mnemonics,
                    args.pc_column,
                    args.opcode_column,
                    args.valid_column,
                    not args.no_header,
                )
        print(f"wrote {count} rows", file=sys.stderr)
    except OSError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
