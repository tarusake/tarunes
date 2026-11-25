#!/usr/bin/env python3
"""Import FreeRouting SES output and save the routed KiCad board."""

from pathlib import Path
import pcbnew

here = Path(__file__).resolve().parent
pcb = here / "primer20k-dock-v3713-fc-serial.kicad_pcb"
ses = here / "primer20k-dock-v3713-fc-serial.ses"
board = pcbnew.LoadBoard(str(pcb))
if not pcbnew.ImportSpecctraSES(board, str(ses)):
    raise SystemExit("failed to import Specctra session")
board.GetDesignSettings().m_CopperEdgeClearance = pcbnew.FromMM(0.30)

# FreeRouting keeps only about 0.289 mm at the straight front edge. Extend
# that non-critical outer edge by 0.02 mm after routing; cartridge and PMOD
# datum edges are unchanged, and the final physical clearance exceeds 0.30 mm.
old_front = pcbnew.FromMM(-0.10)
new_front = pcbnew.FromMM(-0.12)
for item in board.GetDrawings():
    if item.GetLayer() != pcbnew.Edge_Cuts or not isinstance(item, pcbnew.PCB_SHAPE):
        continue
    start = item.GetStart(); end = item.GetEnd()
    if start.y == old_front:
        item.SetStart(pcbnew.VECTOR2I(start.x, new_front))
    if end.y == old_front:
        item.SetEnd(pcbnew.VECTOR2I(end.x, new_front))

# Final assembly metadata is intentionally applied after SES import so a long
# autorouter run does not have to be repeated for non-geometric BOM changes.
for fp in board.GetFootprints():
    ref = fp.GetReference()
    if ref == "J2":
        fp.SetDNP(True)

# A two-layer stack-up uses both sides as GND pours around the routed tracks.
# The final DRC command refills these zones before saving the board.
gnd = board.FindNet("GND")
for layer in (pcbnew.F_Cu, pcbnew.B_Cu):
    zone = pcbnew.ZONE(board)
    zone.SetLayer(layer)
    zone.SetNet(gnd)
    zone.SetLocalClearance(pcbnew.FromMM(0.25))
    zone.SetMinThickness(pcbnew.FromMM(0.20))
    zone.SetPadConnection(pcbnew.ZONE_CONNECTION_FULL)
    outline = zone.Outline()
    outline.NewOutline()
    for x, y in ((0.5, 0.5), (84.5, 0.5), (84.5, 54.5),
                 (59.5, 54.5), (59.5, 44.0),
                 (11.0, 44.0), (11.0, 54.5),
                 (0.5, 54.5)):
        outline.Append(pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y)))
    board.Add(zone)

# Purposeful GND stitching: beside local decoupling groups, the boost/power
# section, PMOD0, and around the perimeter/cutout. DRC validates clearance to
# all signal tracks after zone refill.
for x, y in ((3, 35), (78, 52), (82, 52)):
    via = pcbnew.PCB_VIA(board)
    via.SetNet(gnd)
    via.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y)))
    via.SetWidth(pcbnew.FromMM(0.60))
    via.SetDrill(pcbnew.FromMM(0.30))
    via.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
    board.Add(via)
pcbnew.SaveBoard(str(pcb), board)
print(f"routed board saved to {pcb}")
