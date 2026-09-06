from pathlib import Path
import re

root = Path(__file__).parent

dtsi = (root / "boards/shields/hy73/hy73.dtsi").read_text()
layout = (root / "boards/shields/hy73/hy73-layouts.dtsi").read_text()
keymap = (root / "boards/shields/hy73/hy73.keymap").read_text()

map_block = re.search(r"map = <(.*?)>;", dtsi, re.S).group(1)
rc_count = len(re.findall(r"RC\(", map_block))
physical_count = layout.count("<&key_physical_attrs")

binding_blocks = re.findall(r"bindings = <(.*?)>;", keymap, re.S)
binding_counts = []
for block in binding_blocks:
    # Every binding begins with &, and parameters (e.g. BT_SEL 0) do not.
    binding_counts.append(len(re.findall(r"&[A-Za-z_][A-Za-z0-9_]*", block)))

assert rc_count == 73, rc_count
assert physical_count == 73, physical_count
assert binding_counts == [73, 73], binding_counts

print("Static validation passed")
print(f"transform positions: {rc_count}")
print(f"physical layout keys: {physical_count}")
print(f"keymap binding counts: {binding_counts}")
