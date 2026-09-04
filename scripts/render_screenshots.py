#!/usr/bin/env python3
"""Render sample transcripts as terminal-style PNG screenshots for the lab report."""

from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
SAMPLES = ROOT / "samples"
OUT = ROOT / "report" / "screenshots"
FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
TITLE_FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

BG = (15, 17, 21)
FG = (220, 230, 220)
DIM = (90, 110, 90)
ACCENT = (80, 200, 120)
TITLE_BG = (28, 32, 38)
BORDER = (45, 52, 60)

SHOTS = [
    ("01_series.txt", "01_series.png", "Tool 1 — Series Circuit Solver"),
    ("02_parallel.txt", "02_parallel.png", "Tool 2 — Parallel Circuit Solver"),
    ("03_color_code.txt", "03_color_code.png", "Tool 3 — Resistor Value Determiner"),
    ("04_equivalent_series.txt", "04_equivalent_series.png", "Tool 4 — Equivalent Resistance (series)"),
    ("04_equivalent_parallel.txt", "04_equivalent_parallel.png", "Tool 4 — Equivalent Resistance (parallel)"),
    ("05a_voltage_divider_vout.txt", "05a_voltage_divider_vout.png", "Tool 5a — Output Voltage Determiner"),
    ("05b_voltage_divider_r2.txt", "05b_voltage_divider_r2.png", "Tool 5b — Resistance Calculator"),
]


def wrap_line(text: str, width: int) -> list[str]:
    if len(text) <= width:
        return [text]
    parts = []
    while text:
        parts.append(text[:width])
        text = text[width:]
    return parts


def render(src: Path, dest: Path, title: str) -> None:
    raw = src.read_text(encoding="utf-8").rstrip("\n")
    lines = []
    for line in raw.splitlines() or [""]:
        lines.extend(wrap_line(line.replace("\t", "    "), 78) or [""])

    font = ImageFont.truetype(FONT_PATH, 15)
    title_font = ImageFont.truetype(TITLE_FONT_PATH, 14)
    pad_x, pad_y = 22, 18
    title_h = 36
    line_h = 20
    width = 78 * 9 + pad_x * 2
    height = title_h + pad_y * 2 + line_h * len(lines)

    img = Image.new("RGB", (width, height), BG)
    draw = ImageDraw.Draw(img)
    draw.rectangle([0, 0, width, title_h], fill=TITLE_BG)
    draw.rectangle([0, 0, width - 1, height - 1], outline=BORDER)
    draw.ellipse([12, 12, 22, 22], fill=(232, 77, 77))
    draw.ellipse([28, 12, 38, 22], fill=(230, 180, 60))
    draw.ellipse([44, 12, 54, 22], fill=(80, 180, 90))
    draw.text((68, 10), title, font=title_font, fill=FG)

    y = title_h + pad_y
    for line in lines:
        color = FG
        stripped = line.strip()
        if stripped.startswith("----------") or stripped.startswith("====") or stripped.startswith("----"):
            color = ACCENT
        elif stripped.startswith("Enter your choice") or stripped.endswith(":") or "):" in line[:24]:
            color = FG
        elif stripped.startswith("---"):
            color = ACCENT
        draw.text((pad_x, y), line, font=font, fill=color)
        y += line_h

    dest.parent.mkdir(parents=True, exist_ok=True)
    img.save(dest, "PNG", optimize=True)
    print(f"wrote {dest.relative_to(ROOT)} ({img.size[0]}x{img.size[1]})")


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    for src_name, dest_name, title in SHOTS:
        render(SAMPLES / src_name, OUT / dest_name, title)


if __name__ == "__main__":
    main()
