#!/usr/bin/env python3
"""Generate a human-reviewable schematic from the PCB netlist."""

from pathlib import Path
import pcbnew
import kicad_sch_api as ksa

HERE = Path(__file__).resolve().parent
PCB = HERE / "primer20k-dock-v3713-fc-serial.kicad_pcb"
OUT = HERE / "primer20k-dock-v3713-fc-serial.kicad_sch"

board = pcbnew.LoadBoard(str(PCB))
sch = ksa.create_schematic("Tang Primer 20K Dock FC cartridge reader")
sch.set_paper_size("A2")
sch.set_title_block(
    title="Tang Primer 20K Dock V3713 - FC cartridge reader",
    date="2026-11-22", rev="F", company="tarunes",
    comments={1: "Dock PMOD0 direct plug; PMOD1/2/3 exposed by cutouts",
              2: "Mapper 0 (NROM-256) boot-time PRG/CHR ROM reader",
              3: "J1 user supplied; J2 fitted on PCB bottom"},
)

fps = {fp.GetReference(): fp for fp in board.GetFootprints()}


def fp_id(fp):
    fid = fp.GetFPID()
    return f"{fid.GetLibNickname()}:{fid.GetLibItemName()}"


def pad_nets(fp):
    return {pad.GetNumber(): pad.GetNetname() for pad in fp.Pads()
            if pad.GetNumber() and pad.GetNetname()}


symbols = {
    **{f"U{i}": "74xx:74HC595" for i in range(1, 6)},
    "U7": "74xx:74HC165", "U8": "74xx:74HC165",
    "U9": "74xx:74LS540",
    "U10": "Connector_Generic:Conn_01x05",
    "U11": "Connector_Generic:Conn_01x06",
    "J1": "Connector_Generic:Conn_02x30_Odd_Even",
    "J2": "Connector_Generic:Conn_02x06_Odd_Even",
}
for i in range(1, 3):
    symbols[f"R{i}"] = "Device:R"
for i in range(1, 11):
    symbols[f"C{i}"] = "Device:C"

positions = {"J1": (48, 118)}
positions.update({f"U{i}": (135 + (i - 1) * 55, 62) for i in range(1, 6)})
positions.update({"U7": (160, 155), "U8": (225, 155), "J2": (345, 150),
                  "U9": (155, 250), "U11": (255, 250), "U10": (325, 250)})

# Passives sit beside the circuit they serve, rather than in a parts grid.
positions.update({
    "R1": (135, 180), "R2": (150, 180),
})
for i in range(1, 6):
    positions[f"C{i}"] = (135 + (i - 1) * 55, 92)
positions.update({"C6": (160, 195), "C7": (225, 195),
                  "C8": (155, 295), "C9": (255, 295), "C10": (325, 295)})

components = {}
for ref, pos in positions.items():
    fp = fps[ref]
    components[ref] = sch.components.add(
        symbols[ref], ref, fp.GetValue(), position=pos, footprint=fp_id(fp))

# The Dock connector is the intentional source for the shared ground rail.
# Declare it driven for ERC; this flag is documentation-only and has no PCB
# footprint.
gnd_flag = sch.components.add("power:PWR_FLAG", "#FLG01", "PWR_FLAG", position=(275, 195))
sch.add_label("GND", pin=("#FLG01", "1"), size=0.8)

def pin_pos(ref, pin):
    # Resolve through the schematic manager so symbol transforms are applied
    # exactly as they are when KiCad evaluates connectivity.
    return sch.get_component_pin_position(ref, str(pin))


def wire_label(ref, pin, name):
    """Attach an outward-facing wire stub and net label to a pin.

    Label rotation matters: leaving every label at zero degrees makes the
    KiCad label flags point back through left/top pins, producing apparent
    wire crossings around dense ICs and the cartridge connector.
    """
    p = pin_pos(ref, pin)
    # Pin rotation describes the direction from the connection point into the
    # symbol body.  Extend in the opposite direction.  Inferring direction
    # from the symbol centre fails on tall connectors such as J1.
    pin_rotation = int(components[ref].get_pin(str(pin)).rotation) % 360
    if pin_rotation == 0:       # left side
        end = (p.x - 7.62, p.y)
        sch.add_wire(end, (p.x, p.y))
        rotation = 180
    elif pin_rotation == 180:  # right side
        end = (p.x + 7.62, p.y)
        sch.add_wire(end, (p.x, p.y))
        rotation = 0
    else:
        # Turn top/bottom connections sideways so their names remain
        # horizontal and can be read without rotating the page.
        y = p.y - 5.08 if pin_rotation == 270 else p.y + 5.08
        corner = (p.x, y)
        end = (p.x + 10.16, y)
        sch.add_wire((p.x, p.y), corner)
        sch.add_wire(corner, end)
        rotation = 0
    sch.add_label(name, position=end, rotation=rotation, size=0.9)


# Every connection receives a short named stub. This avoids long bus wires
# crossing unrelated pins while keeping the shift-chain net names explicit.
direct_pins = set()


def orthogonal_pin_wire(ref1, pin1, ref2, pin2):
    """Draw an explicit three-segment connection between functional stages."""
    a = pin_pos(ref1, pin1)
    b = pin_pos(ref2, pin2)
    mid_x = (a.x + b.x) / 2
    sch.add_wire((a.x, a.y), (mid_x, a.y))
    sch.add_wire((mid_x, a.y), (mid_x, b.y))
    sch.add_wire((mid_x, b.y), (b.x, b.y))
    direct_pins.update({(ref1, str(pin1)), (ref2, str(pin2))})


# Make the serial signal flow visible without requiring label lookup.
for left, right in (("U1", "U2"), ("U2", "U3"),
                    ("U3", "U4"), ("U4", "U5")):
    orthogonal_pin_wire(left, 9, right, 14)
# PPU byte shifts into the CPU-byte register, then U7 drives PMOD MISO.
orthogonal_pin_wire("U8", 7, "U7", 10)

# Nets that exist on only one PCB pad are deliberately unused.  Show a real
# no-connect cross instead of inventing another net label; this removes the
# forest of SPARE/NC names while preserving the electrical intent.
net_pad_count = {}
for fp in board.GetFootprints():
    for pad in fp.Pads():
        if pad.GetNetname():
            net_pad_count[pad.GetNetname()] = net_pad_count.get(pad.GetNetname(), 0) + 1

# Named wire stubs keep the 60-pin FC buses readable without hiding connectivity.
for ref, comp in components.items():
    nets = pad_nets(fps[ref])
    valid = {str(info["number"]) for info in comp.list_pins()}
    for pin in sorted(valid, key=lambda value: int(value) if value.isdigit() else 999):
        if (ref, pin) in direct_pins:
            continue
        name = nets.get(pin)
        if name and net_pad_count.get(name, 0) == 1:
            p = pin_pos(ref, pin)
            sch.no_connects.add((p.x, p.y))
        elif name:
            wire_label(ref, pin, name)
        else:
            p = pin_pos(ref, pin)
            sch.no_connects.add((p.x, p.y))

for text, pos, size in (
    ("J1 — FC 60-PIN CARTRIDGE", (20, 35), 2.2),
    ("ADDRESS / CONTROL OUTPUT — FIVE 74HC595", (120, 35), 2.2),
    ("SERIAL FLOW  U1  →  U2  →  U3  →  U4  →  U5", (180, 105), 1.1),
    ("PRG / CHR DATA INPUT — TWO 74HC165", (125, 120), 2.2),
    ("SERIAL FLOW  U8 (PPU)  →  U7 (CPU)  →  R1/R2  →  PMOD MISO", (145, 207), 1.1),
    ("J2 — DOCK PMOD0 SERIAL INTERFACE", (315, 120), 2.2),
    ("POWER AND 3.3 V / 5 V LEVEL TRANSLATION", (120, 220), 2.2),
    ("U11: 3.3 V to 5 V boost     U10: switched cartridge supply", (235, 220), 1.1),
    ("U9 is inverting: firmware emits inverted MOSI/SCLK/RCLK/PL; power enable also controls /OE",
     (230, 310), 1.1),
):
    sch.add_text(text, position=pos, size=size)

sch.save_as(OUT, preserve_format=False)
print(f"wrote {OUT}")
