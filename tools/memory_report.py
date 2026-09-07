#!/usr/bin/env python3
"""Extract Zephyr linker memory usage from a captured build log."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


UNITS = {
    "B": 1,
    "KB": 1024,
    "MB": 1024 * 1024,
    "GB": 1024 * 1024 * 1024,
}

PATTERN = re.compile(
    r"^\s*(FLASH|RAM):\s+"
    r"([0-9]+(?:\.[0-9]+)?)\s+(B|KB|MB|GB)\s+"
    r"([0-9]+(?:\.[0-9]+)?)\s+(B|KB|MB|GB)\s+"
    r"([0-9]+(?:\.[0-9]+)?)%\s*$"
)


def to_bytes(value: str, unit: str) -> int:
    return int(round(float(value) * UNITS[unit]))


def parse_log(text: str) -> dict[str, dict[str, int | float]]:
    result: dict[str, dict[str, int | float]] = {}

    for line in text.splitlines():
        match = PATTERN.match(line)
        if not match:
            continue

        region, used_value, used_unit, total_value, total_unit, percent = match.groups()
        used = to_bytes(used_value, used_unit)
        total = to_bytes(total_value, total_unit)
        result[region] = {
            "used_bytes": used,
            "total_bytes": total,
            "free_bytes": total - used,
            "percent": float(percent),
        }

    missing = {"FLASH", "RAM"} - result.keys()
    if missing:
        raise SystemExit(f"Could not find memory regions in build log: {', '.join(sorted(missing))}")

    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("build_log", type=Path)
    parser.add_argument("--name", required=True)
    parser.add_argument("--json", dest="json_path", type=Path, required=True)
    parser.add_argument("--markdown", dest="markdown_path", type=Path, required=True)
    args = parser.parse_args()

    regions = parse_log(args.build_log.read_text(encoding="utf-8", errors="replace"))
    payload = {"name": args.name, "regions": regions}
    args.json_path.write_text(json.dumps(payload, indent=4) + "\n", encoding="utf-8")

    lines = [
        f"### {args.name}",
        "",
        "| Region | Used | Total | Free | Used % |",
        "| --- | ---: | ---: | ---: | ---: |",
    ]
    for region in ("FLASH", "RAM"):
        data = regions[region]
        lines.append(
            f"| {region} | {data['used_bytes']:,} B | {data['total_bytes']:,} B | "
            f"{data['free_bytes']:,} B | {data['percent']:.2f}% |"
        )

    args.markdown_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
