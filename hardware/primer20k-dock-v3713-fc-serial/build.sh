#!/usr/bin/env bash
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
board="$here/primer20k-dock-v3713-fc-serial.kicad_pcb"
schematic="$here/primer20k-dock-v3713-fc-serial.kicad_sch"
out="$here/manufacturing"
router="${FREEROUTING_JAR:-/tmp/freerouting.jar}"
mkdir -p "$out/gerbers"

python3 "$here/generate_board.py"
java -jar "$router" --gui.enabled=false -mt 1 \
  -de "$here/primer20k-dock-v3713-fc-serial.dsn" \
  -do "$here/primer20k-dock-v3713-fc-serial.ses" -mp 30 -da
python3 "$here/import_routing.py"
python3 "$here/generate_schematic.py"
kicad-cli sch erc --severity-error --exit-code-violations \
  --output "$out/erc.rpt" "$schematic"
kicad-cli sch export pdf \
  --output "$here/primer20k-dock-v3713-fc-serial-schematic.pdf" "$schematic"
kicad-cli pcb drc --severity-error --exit-code-violations --refill-zones --save-board \
  --output "$out/drc.rpt" "$board"
python3 "$here/generate_manufacturing.py"
kicad-cli pcb export gerbers --output "$out/gerbers" \
  --layers F.Cu,B.Cu,F.Mask,B.Mask,F.Silkscreen,B.Silkscreen,Edge.Cuts \
  --exclude-value --subtract-soldermask "$board"
kicad-cli pcb export drill --output "$out/gerbers" --format excellon \
  --excellon-units mm --excellon-separate-th "$board"
(cd "$out/gerbers" && zip -q -FS ../gerbers.zip ./*)

echo "Manufacturing package: $out"
