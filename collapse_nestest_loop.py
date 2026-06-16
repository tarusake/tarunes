#!/usr/bin/env python3
import argparse
import re
import sys
from pathlib import Path
from typing import Iterable, NamedTuple, TextIO


class LoopSpec(NamedTuple):
    label: str
    patterns: tuple[re.Pattern[str], ...]


LOOP_SPECS = (
    LoopSpec(
        "C28F/C291",
        (
            re.compile(r"^C28F\s+CMP\s+\$D2\s+=\s+\$[0-9A-Fa-f]{2}\b"),
            re.compile(r"^C291\s+BEQ\s+\$C28F\b"),
        ),
    ),
    LoopSpec(
        "C009/C00C",
        (
            re.compile(r"^C009\s+LDA\s+PpuStatus_2002\s+=\s+\$[0-9A-Fa-f]{2}\b"),
            re.compile(r"^C00C\s+BPL\s+\$C009\b"),
        ),
    ),
    LoopSpec(
        "800A/800D",
        (
            re.compile(r"^800A\s+LDA\s+PpuStatus_2002\s+=\s+\$[0-9A-Fa-f]{2}\b"),
            re.compile(r"^800D\s+BPL\s+\$800A\b"),
        ),
    ),
    LoopSpec(
        "800F/8012",
        (
            re.compile(r"^800F\s+LDA\s+PpuStatus_2002\s+=\s+\$[0-9A-Fa-f]{2}\b"),
            re.compile(r"^8012\s+BPL\s+\$800F\b"),
        ),
    ),
    LoopSpec(
        "8057",
        (
            re.compile(r"^8057\s+JMP\s+\$8057\b"),
        ),
    ),
    LoopSpec(
        "813D/8140/8142",
        (
            re.compile(r"^813D\s+LDA\s+PpuStatus_2002\s+=\s+\$[0-9A-Fa-f]{2}\b"),
            re.compile(r"^8140\s+AND\s+#\$40\b"),
            re.compile(r"^8142\s+BNE\s+\$813D\b"),
        ),
    ),
    LoopSpec(
        "8150/8153/8155",
        (
            re.compile(r"^8150\s+LDA\s+PpuStatus_2002\s+=\s+\$[0-9A-Fa-f]{2}\b"),
            re.compile(r"^8153\s+AND\s+#\$40\b"),
            re.compile(r"^8155\s+BEQ\s+\$8150\b"),
        ),
    ),
    LoopSpec(
        "8E3B/8E3F",
        (
            re.compile(r"^8E3B\s+STA\s+PpuData_2007\b"),
            re.compile(r"^8E3F\s+BNE\s+\$8E3B\b"),
        ),
    ),
    LoopSpec(
        "C034/C037/C038",
        (
            re.compile(r"^C034\s+STA\s+PpuData_2007\b"),
            re.compile(r"^C037\s+DEX\b"),
            re.compile(r"^C038\s+BNE\s+\$C034\b"),
        ),
    ),
)
CYCLE_RE = re.compile(r"\bCycle:(\d+)\b")


def cycle_of(line: str) -> str:
    match = CYCLE_RE.search(line)
    return match.group(1) if match else "?"


class LoopCollapser:
    def __init__(self, output: TextIO, min_iterations: int) -> None:
        self.output = output
        self.min_iterations = min_iterations
        self.pending_lines: list[str] = []
        self.pending_spec: LoopSpec | None = None
        self.count = 0
        self.active_spec: LoopSpec | None = None
        self.first_iteration: tuple[str, ...] | None = None
        self.last_iteration: tuple[str, ...] | None = None
        self.buffered_iterations: list[tuple[str, ...]] = []

    def process(self, lines: Iterable[str]) -> None:
        for line in lines:
            while True:
                consumed = self.process_line(line)
                if consumed:
                    break

        self.flush_pending()
        self.flush_loop()

    def process_line(self, line: str) -> bool:
        if self.pending_lines:
            assert self.pending_spec is not None
            next_pattern = self.pending_spec.patterns[len(self.pending_lines)]
            if next_pattern.match(line):
                self.pending_lines.append(line)
                if len(self.pending_lines) == len(self.pending_spec.patterns):
                    self.add_loop_iteration(self.pending_spec, tuple(self.pending_lines))
                    self.pending_lines.clear()
                    self.pending_spec = None
                return True

            self.flush_pending()
            return False

        spec = matching_spec(line)
        if spec is not None:
            self.pending_lines = [line]
            self.pending_spec = spec
            if len(spec.patterns) == 1:
                assert self.pending_spec is not None
                self.add_loop_iteration(self.pending_spec, tuple(self.pending_lines))
                self.pending_lines.clear()
                self.pending_spec = None
            return True

        self.flush_loop()
        self.output.write(line)
        return True

    def flush_pending(self) -> None:
        if not self.pending_lines:
            return

        self.flush_loop()
        for line in self.pending_lines:
            self.output.write(line)
        self.pending_lines.clear()
        self.pending_spec = None

    def add_loop_iteration(self, spec: LoopSpec, iteration: tuple[str, ...]) -> None:
        if self.active_spec is not None and self.active_spec != spec:
            self.flush_loop()

        self.active_spec = spec
        if self.count == 0:
            self.first_iteration = iteration

        self.count += 1
        self.last_iteration = iteration

        if self.count < self.min_iterations:
            self.buffered_iterations.append(iteration)
        elif self.count == self.min_iterations:
            self.buffered_iterations.clear()

    def flush_loop(self) -> None:
        if self.count == 0:
            return

        if self.count < self.min_iterations:
            for iteration in self.buffered_iterations:
                self.write_iteration(iteration)
        else:
            assert self.first_iteration is not None
            assert self.last_iteration is not None
            self.write_iteration(self.first_iteration)

            omitted = self.count - 2
            if omitted > 0:
                assert self.active_spec is not None
                self.output.write(
                    f"... {self.active_spec.label} loop omitted: {omitted} iterations "
                    f"(first Cycle:{cycle_of(self.first_iteration[0])}, "
                    f"last Cycle:{cycle_of(self.last_iteration[-1])})\n"
                )

            if self.count > 1:
                self.write_iteration(self.last_iteration)

        self.count = 0
        self.active_spec = None
        self.first_iteration = None
        self.last_iteration = None
        self.buffered_iterations.clear()

    def write_iteration(self, iteration: tuple[str, ...]) -> None:
        for line in iteration:
            self.output.write(line)


def matching_spec(line: str) -> LoopSpec | None:
    for spec in LOOP_SPECS:
        if spec.patterns[0].match(line):
            return spec
    return None


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Collapse long nestest wait loops in a Mesen-style CPU trace."
    )
    parser.add_argument("input", nargs="?", type=Path, default=Path("nestest.log"))
    parser.add_argument("-o", "--output", type=Path, help="write collapsed log to this file")
    parser.add_argument(
        "--min-pairs",
        type=int,
        default=3,
        help="minimum consecutive loop iterations to collapse (default: 3)",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.min_pairs < 2:
        print("--min-pairs must be at least 2", file=sys.stderr)
        return 2

    with args.input.open("r", encoding="utf-8", errors="replace") as input_file:
        if args.output is None:
            collapser = LoopCollapser(sys.stdout, args.min_pairs)
            collapser.process(input_file)
        else:
            with args.output.open("w", encoding="utf-8", newline="") as output_file:
                collapser = LoopCollapser(output_file, args.min_pairs)
                collapser.process(input_file)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
