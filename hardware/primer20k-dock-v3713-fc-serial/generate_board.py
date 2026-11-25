#!/usr/bin/env python3
"""Generate the Primer 20K Dock V3713 FC serial-reader PCB.

The script is the placement/netlist source of truth.  Routing is performed from
the emitted Specctra DSN and imported back into this board by build.sh.
"""

from __future__ import annotations

import csv
from pathlib import Path

import pcbnew

HERE = Path(__file__).resolve().parent
FP = Path("/usr/share/kicad/footprints")
OUT = HERE / "primer20k-dock-v3713-fc-serial.kicad_pcb"
DSN = HERE / "primer20k-dock-v3713-fc-serial.dsn"


def mm(v: float) -> int:
    return pcbnew.FromMM(v)


def pt(x: float, y: float) -> pcbnew.VECTOR2I:
    return pcbnew.VECTOR2I(mm(x), mm(y))


board = pcbnew.BOARD()
board.SetCopperLayerCount(2)
board.GetDesignSettings().SetBoardThickness(mm(1.6))


def net(name: str) -> pcbnew.NETINFO_ITEM:
    n = board.FindNet(name)
    if n:
        return n
    n = pcbnew.NETINFO_ITEM(board, name)
    board.Add(n)
    return n


for name in ("GND", "+3V3", "+5V_RAW", "+5V_CART"):
    net(name)


def load(lib: str, name: str, ref: str, value: str, x: float, y: float,
         rot: float = 0.0, dnp: bool = False) -> pcbnew.FOOTPRINT:
    f = pcbnew.FootprintLoad(str(FP / f"{lib}.pretty"), name)
    if f is None:
        raise RuntimeError(f"missing footprint {lib}:{name}")
    # Current KiCad 3D packages ship STEP models.  Some distro footprints still
    # reference legacy KICAD6/7 variables and WRL names, so normalize them to a
    # project-independent variable used by the render command.
    for model in f.Models():
        model.m_Filename = model.m_Filename.replace("${KICAD6_3DMODEL_DIR}", "${KICAD10_3DMODEL_DIR}")
        model.m_Filename = model.m_Filename.replace("${KICAD7_3DMODEL_DIR}", "${KICAD10_3DMODEL_DIR}")
        if model.m_Filename.endswith(".wrl"):
            model.m_Filename = model.m_Filename[:-4] + ".step"
    f.SetReference(ref)
    f.SetValue(value)
    f.SetPosition(pt(x, y))
    f.SetOrientationDegrees(rot)
    f.SetDNP(dnp)
    # Assembly side: show designators, but keep long procurement values off
    # the production silkscreen.
    f.Reference().SetVisible(True)
    f.Value().SetVisible(False)
    board.Add(f)
    return f


def setp(f: pcbnew.FOOTPRINT, pin: str | int, name: str) -> None:
    pads = [p for p in f.Pads() if p.GetNumber() == str(pin)]
    if not pads:
        raise RuntimeError(f"{f.GetReference()} missing pad {pin}")
    for p in pads:
        p.SetNet(net(name))


def fc_slot() -> pcbnew.FOOTPRINT:
    f = pcbnew.FOOTPRINT(board)
    # Use an installed library identifier so Specctra round-tripping can name
    # the custom board-resident footprint without requiring a private fp-lib.
    f.SetFPID(pcbnew.LIB_ID("Connector_PinHeader_2.54mm", "PinHeader_2x30_P2.54mm_Vertical"))
    f.SetReference("J1")
    f.SetValue("FC_60P_SLOT_ALIEXPRESS_1005007638566482_E")
    f.SetDNP(True)
    f.SetPosition(pt(42.5, 5.5))
    f.Reference().SetVisible(False)
    f.Value().SetVisible(False)
    # Viewed from above with the cartridge label facing the front board edge.
    for col in range(30):
        x = (col - 14.5) * 2.54
        for y, number in ((-2.54, col + 1), (2.54, col + 31)):
            p = pcbnew.PAD(f)
            p.SetNumber(str(number))
            p.SetAttribute(pcbnew.PAD_ATTRIB_PTH)
            p.SetShape(pcbnew.PAD_SHAPE_CIRCLE)
            p.SetSize(pt(1.8, 1.8))
            p.SetDrillSize(pt(1.0, 1.0))
            p.SetFPRelativePosition(pt(x, y))
            p.SetLayerSet(p.PTHMask())
            f.Add(p)
    board.Add(f)
    return f


def dock_header(ref: str, x: float, value: str) -> pcbnew.FOOTPRINT:
    return load("Connector_PinHeader_2.54mm", "PinHeader_2x06_P2.54mm_Vertical",
                ref, value, x, 50, -90, dnp=True)


def resistor(ref: str, value: str, x: float, y: float, rot: float = 0) -> pcbnew.FOOTPRINT:
    return load("Resistor_THT", "R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal",
                ref, value, x, y, rot)


def capacitor(ref: str, value: str, x: float, y: float, size: str = "0603", rot: float = 0) -> pcbnew.FOOTPRINT:
    fpname = "C_0805_2012Metric" if size == "0805" else "C_0603_1608Metric"
    return load("Capacitor_SMD", fpname, ref, value, x, y, rot)


def akizuki_boost_module(ref: str, x: float, y: float) -> pcbnew.FOOTPRINT:
    """AE-XCL102/103 DIP6 module, 2.54 mm pitch and 7.62 mm rows."""
    f = load("Package_DIP", "DIP-6_W7.62mm", ref,
             "AE-XCL102D503CR-G", x, y)
    return f


fc = fc_slot()
pinmap = {}
with (HERE / "shift-map.csv").open(newline="") as h:
    for row in csv.DictReader(h):
        if row["fc_pin"] and row["direction"] != "TO_CART_WRITE":
            pinmap[int(row["fc_pin"])] = row["signal"]
for pin, signal in pinmap.items():
    setp(fc, pin, signal)
for pin in (1, 16):
    setp(fc, pin, "GND")
setp(fc, 30, "+5V_CART")
# Unused cartridge functions are intentionally grounded only where safe; all
# other unused pins have no pad net and are documented in README.

# Five output shift registers provide the 35 address/control outputs needed by
# an NROM reader.  The remaining five outputs are deliberately unused.
q_pins = (15, 1, 2, 3, 4, 5, 6, 7)
outs = []
with (HERE / "shift-map.csv").open(newline="") as h:
    outs = [r for r in csv.DictReader(h) if r["chain"] == "OUT"]
out_positions = ((10, 16), (30, 16), (50, 16), (20, 28), (40, 28))
for i, (x, y) in enumerate(out_positions, 1):
    u = load("Package_SO", "SOIC-16_3.9x9.9mm_P1.27mm", f"U{i}",
             "TC74HC595AF", x, y, 90)
    setp(u, 8, "GND"); setp(u, 16, "+5V_RAW")
    setp(u, 10, "+5V_RAW"); setp(u, 11, "CART_SCLK")
    setp(u, 12, "CART_RCLK"); setp(u, 13, "SHIFT_OE_N")
    setp(u, 14, "CART_MOSI" if i == 1 else f"SHIFT_OUT_{i-1}_{i}")
    setp(u, 9, f"SHIFT_OUT_{i}_{i+1}" if i < 5 else "SHIFT_OUT_END")
    for j, p in enumerate(q_pins):
        row = outs[(i - 1) * 8 + j]
        if row["direction"] == "UNUSED":
            name = f"SPARE{row['bit']}"
        else:
            name = row["signal"]
        setp(u, p, name)

# Two input shift registers.  74HCT165 shifts H..A, so reverse the D pin order
# to make the serialized sequence CPU_D0..D7 then PPU_D0..D7.
d_serial_order = (6, 5, 4, 3, 14, 13, 12, 11)
ins = []
with (HERE / "shift-map.csv").open(newline="") as h:
    ins = [r for r in csv.DictReader(h) if r["chain"] == "IN"]
for i, (x, y) in zip((7, 8), ((58, 18), (50, 34))):
    u = load("Package_DIP", "DIP-16_W7.62mm", f"U{i}",
             "TC74HC165AP", x, y, 90)
    setp(u, 1, "CART_PL_N"); setp(u, 2, "CART_SCLK")
    setp(u, 7, "PISO_Q_CPU" if i == 7 else "PISO_Q_PPU")
    setp(u, 8, "GND"); setp(u, 9, f"PISO_QN_{i}")
    setp(u, 10, "PISO_Q_PPU" if i == 7 else "GND")
    setp(u, 15, "GND"); setp(u, 16, "+5V_RAW")
    base = 0 if i == 7 else 8
    for j, p in enumerate(d_serial_order):
        setp(u, p, ins[base + j]["signal"])

# Only Dock PMOD0 is occupied. PMOD1/2/3 remain accessible through edge
# cutouts so other Dock modules (including PMOD-SFC) plug in directly.
j2 = dock_header("J2", 77.30, "DOCK_PMOD0_SERIAL")

# Standard odd/even footprint pads translated to the reference extender's
# physical PMOD order (pins 1..6 first row, pins 7..12 second row).
j2_map = {1: "PMOD_NC1", 3: "CART_MISO", 5: "CART_PL_3V3",
          7: "CART_MOSI_3V3", 9: "GND", 11: "+3V3",
          2: "CART_PWR_EN", 4: "PMOD_NC2", 6: "CART_RCLK",
          8: "CART_SCLK_3V3", 10: "GND", 12: "+3V3"}
for p, n in j2_map.items(): setp(j2, p, n)

# MISO 5 V to 3.3 V divider.
r1 = resistor("R1", "1k", 80, 20, 90); setp(r1, 1, "PISO_Q_CPU"); setp(r1, 2, "CART_MISO")
r2 = resistor("R2", "2k", 83, 28, 90); setp(r2, 1, "CART_MISO"); setp(r2, 2, "GND")

# One 5 V TTL-input buffer translates all PMOD control signals.  The 540 is
# inverting; firmware therefore emits inverted MOSI/clock/latch waveforms.
u9 = load("Package_SO", "SOIC-20W_7.5x12.8mm_P1.27mm", "U9",
          "TC74VHCT540AF", 78, 38, 0)
for pin in (1, 19): setp(u9, pin, "GND")
setp(u9, 20, "+5V_RAW"); setp(u9, 10, "GND")
for pin, name in ((2, "CART_MOSI_3V3"), (3, "CART_SCLK_3V3"),
                  (4, "CART_RCLK"), (5, "CART_PL_3V3"),
                  (6, "CART_PWR_EN"), (7, "GND"), (8, "GND"), (9, "GND"),
                  (18, "CART_MOSI"), (17, "CART_SCLK"),
                  (16, "CART_RCLK_5V"), (15, "CART_PL_N"),
                  (14, "SHIFT_OE_N"), (13, "LEVEL_NC6"),
                  (12, "LEVEL_NC7"), (11, "LEVEL_NC8")):
    setp(u9, pin, name)
# Keep the existing internal net name used by the 595 latch pins.
for uref in ("U1", "U2", "U3", "U4", "U5"):
    setp(board.FindFootprintByReference(uref), 12, "CART_RCLK_5V")

# Hand-solderable Akizuki DIP6 boost module. Pins 3 and 6 are both VIN, but
# pin 6 is the main current path according to the module documentation.
# Keep the module body comfortably inside the PMOD cutout edge.  At y=38 its
# courtyard/body extended fractionally across the y=44.5 mm notch datum.
u11 = akizuki_boost_module("U11", 27, 35)
for p, n in ((1, "GND"), (2, "+3V3"), (3, "+3V3"),
             (4, "+5V_RAW"), (5, "GND"), (6, "+3V3")): setp(u11, p, n)

# Positive-logic Akizuki load switch; pin 4 is NC.
u10 = load("Package_TO_SOT_SMD", "SOT-23-5", "U10", "TCK107AF", 45, 37, 90)
for p, n in ((1, "+5V_RAW"), (2, "GND"), (3, "CART_PWR_EN"),
             (4, "LOADSW_NC"), (5, "+5V_CART")): setp(u10, p, n)

# Decoupling: one 0805 capacitor per logic IC plus load-switch bulk bypass.
decoupling_positions = ((18, 16), (38, 16), (36, 21),
                        (28, 28), (42, 33), (45, 22), (40, 38), (69, 42))
for i, (x, y) in enumerate(decoupling_positions, 1):
    c = capacitor(f"C{i}", "1uF", x, y, "0805")
    setp(c, 1, "+5V_RAW"); setp(c, 2, "GND")
for ref, value, x, y, rail in (("C9", "1uF", 39, 36, "+5V_RAW"),
                                ("C10", "22uF", 52, 40, "+5V_CART")):
    c = capacitor(ref, value, x, y, "0805")
    setp(c, 1, rail); setp(c, 2, "GND")

# Hand-tuned reference positions keep the dense power-area silkscreen
# readable after switching to hand-assembly footprints.
for ref, x, y in (("C5", 45.0, 32.0), ("C7", 38.0, 40.5),
                  ("C9", 36.0, 33.0), ("R1", 82.0, 20.0),
                  ("U7", 66.0, 19.0), ("U10", 45.0, 34.0)):
    board.FindFootprintByReference(ref).Reference().SetPosition(pt(x, y))

# Board outline. One continuous notch exposes Dock PMOD1/2/3.
outline = ((0, -0.1), (85, -0.1), (85, 55),
           (59.0, 55), (59.0, 44.5), (11.5, 44.5), (11.5, 55),
           (0, 55), (0, -0.1))
for a, b in zip(outline, outline[1:]):
    s = pcbnew.PCB_SHAPE(board)
    s.SetShape(pcbnew.SHAPE_T_SEGMENT); s.SetStart(pt(*a)); s.SetEnd(pt(*b))
    s.SetWidth(mm(0.05)); s.SetLayer(pcbnew.Edge_Cuts); board.Add(s)

# Keep routing at least 0.30 mm away from the inside corner of the PMOD
# cutout.  Specctra otherwise tends to shave this corner despite KiCad's
# stricter copper-to-edge rule.
for layer in (pcbnew.F_Cu, pcbnew.B_Cu):
    for corners in (
            ((58.3, 43.8), (59.7, 43.8), (59.7, 45.2), (58.3, 45.2)),
            ((84.0, 32.2), (85.5, 32.2), (85.5, 34.8), (84.0, 34.8))):
        keepout = pcbnew.ZONE(board)
        keepout.SetLayer(layer)
        keepout.SetIsRuleArea(True)
        keepout.SetDoNotAllowTracks(True)
        keepout.SetDoNotAllowVias(True)
        polygon = keepout.Outline(); polygon.NewOutline()
        for x, y in corners:
            polygon.Append(pt(x, y))
        board.Add(keepout)

# Identification and assembly notes.  These labels remain in the Gerber;
# component values are intentionally omitted, while reference designators are
# exported for hand inspection and rework.
def silk_text(text: str, x: float, y: float, size: float = 1.0,
              layer: int = pcbnew.F_SilkS, rot: float = 0.0) -> None:
    t = pcbnew.PCB_TEXT(board); t.SetText(text); t.SetPosition(pt(x, y))
    t.SetLayer(layer); t.SetTextSize(pt(size, size)); t.SetTextThickness(mm(0.18))
    if layer == pcbnew.B_SilkS:
        t.SetMirrored(True)
    t.SetTextAngle(pcbnew.EDA_ANGLE(rot, pcbnew.DEGREES_T)); board.Add(t)


for text, x, y, size, layer, rot in (
        ("tarunes FC READER", 42.5, 10.0, 1.0, pcbnew.F_SilkS, 0),
        ("V3713 REV F", 42.5, 11.5, 0.8, pcbnew.F_SilkS, 0),
        ("PIN 1", 4.0, 2.0, 0.8, pcbnew.F_SilkS, 90),
        ("PIN 30", 81.0, 2.0, 0.8, pcbnew.F_SilkS, 90),
        ("NO HOT-PLUG", 7.0, 42.5, 0.8, pcbnew.F_SilkS, 90),
        ):
    silk_text(text, x, y, size, layer, rot)

# Rules suitable for JLCPCB standard 2-layer service.
ds = board.GetDesignSettings()
ds.m_MinClearance = mm(0.15)
ds.m_CopperEdgeClearance = mm(0.30)
board.GetAllNetClasses()["Default"].SetClearance(mm(0.15))
ds.m_TrackMinWidth = mm(0.20)
ds.m_ViasMinSize = mm(0.60)
ds.m_ViasMinDrill = mm(0.30)

# The boost module can draw roughly 0.4 A from 3V3 at full cartridge load.
# Give supply rails explicit widths instead of the 0.20 mm signal default.
# 0.60/0.40 mm remains routable through the dense SOIC fan-out; 0.80 mm did
# not leave a legal route through this two-layer placement.
settings = ds.m_NetSettings
power_in = pcbnew.NETCLASS("Power_3V3")
power_in.SetClearance(mm(0.15)); power_in.SetTrackWidth(mm(0.60))
power_in.SetViaDiameter(mm(0.60)); power_in.SetViaDrill(mm(0.30))
settings.SetNetclass("Power_3V3", power_in)
settings.SetNetclassPatternAssignment("+3V3", "Power_3V3")
net("+3V3").SetNetClass(power_in)
power_5v = pcbnew.NETCLASS("Power_5V")
power_5v.SetClearance(mm(0.15)); power_5v.SetTrackWidth(mm(0.40))
power_5v.SetViaDiameter(mm(0.60)); power_5v.SetViaDrill(mm(0.30))
settings.SetNetclass("Power_5V", power_5v)
for name in ("+5V_RAW", "+5V_CART"):
    settings.SetNetclassPatternAssignment(name, "Power_5V")
    net(name).SetNetClass(power_5v)

pcbnew.SaveBoard(str(OUT), board)
pcbnew.ExportSpecctraDSN(board, str(DSN))
print(f"wrote {OUT}")
print(f"wrote {DSN}")
