#!/usr/bin/env python3
"""Generate JLCPCB BOM/CPL and a manual-assembly list from the final PCB."""

from __future__ import annotations

import csv
import re
from pathlib import Path

import pcbnew

HERE = Path(__file__).resolve().parent
PCB = HERE / "primer20k-dock-v3713-fc-serial.kicad_pcb"
SOURCE = HERE / "bom-source.csv"
OUT = HERE / "manufacturing"


def expand_references(text: str) -> list[str]:
    refs: list[str] = []
    for token in text.split():
        match = re.fullmatch(r"([A-Z]+)(\d+)-([A-Z]+)?(\d+)", token)
        if not match:
            refs.append(token)
            continue
        p1, first, p2, last = match.groups()
        if p2 and p1 != p2:
            raise ValueError(f"mixed reference range: {token}")
        refs.extend(f"{p1}{number}" for number in range(int(first), int(last) + 1))
    return refs


board = pcbnew.LoadBoard(str(PCB))
footprints = {fp.GetReference(): fp for fp in board.GetFootprints()}
OUT.mkdir(exist_ok=True)

assembled: list[dict[str, str]] = []
manual: list[dict[str, str]] = []
with SOURCE.open(newline="", encoding="utf-8") as handle:
    rows = list(csv.DictReader(handle))

for row in rows:
    refs = expand_references(row["references"])
    if len(refs) != int(row["quantity"]):
        raise ValueError(f"quantity mismatch for {row['references']}")
    missing = set(refs) - footprints.keys()
    if missing:
        raise ValueError(f"PCB has no footprints for {sorted(missing)}")
    if row["assembly"] in {"JLCPCB", "JLCPCB_GLOBAL"}:
        assembled.append(row)
    else:
        manual.append(row)

with (OUT / "bom.csv").open("w", newline="", encoding="utf-8-sig") as handle:
    writer = csv.writer(handle)
    writer.writerow(["Comment", "Designator", "Footprint", "LCSC Part #"])
    for row in assembled:
        refs = expand_references(row["references"])
        writer.writerow([
            row["mpn"], ",".join(refs), row["package"], row["jlcpcb_part"]
        ])

with (OUT / "cpl.csv").open("w", newline="", encoding="utf-8-sig") as handle:
    writer = csv.writer(handle)
    writer.writerow(["Designator", "Mid X", "Mid Y", "Layer", "Rotation"])
    for row in assembled:
        for ref in expand_references(row["references"]):
            fp = footprints[ref]
            pos = fp.GetPosition()
            writer.writerow([
                ref,
                f"{pcbnew.ToMM(pos.x):.4f}mm",
                f"{pcbnew.ToMM(pos.y):.4f}mm",
                "Bottom" if fp.IsFlipped() else "Top",
                f"{fp.GetOrientationDegrees() % 360:.2f}",
            ])

with (OUT / "manual-parts.csv").open("w", newline="", encoding="utf-8-sig") as handle:
    writer = csv.DictWriter(handle, fieldnames=rows[0].keys())
    writer.writeheader()
    writer.writerows(manual)

print(f"wrote {OUT / 'bom.csv'} ({len(assembled)} line items)")
print(f"wrote {OUT / 'cpl.csv'}")
print(f"wrote {OUT / 'manual-parts.csv'}")
