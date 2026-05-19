#!/usr/bin/env python3
"""Generate a terminal screenshot SVG from hardcoded demo output."""

FONT = "Menlo, Monaco, 'Courier New', monospace"
FONT_SIZE = 13
LINE_HEIGHT = 20
PAD_X = 16
PAD_Y = 14
CHAR_W = 7.8   # approximate width per character at font-size 13

BG      = "#1e1e2e"
FG      = "#cdd6f4"
GREEN   = "#a6e3a1"
YELLOW  = "#f9e2af"
RED     = "#f38ba8"
MAGENTA = "#cba6f7"
BOLD_FG = "#ffffff"
DIM_FG  = "#6c7086"

# (text, fg_color, bold)
LINES = [
    # header
    [("┌─────────────────────────────────────────────────────────────┐", BOLD_FG, True)],
    [("│      REAL-TIME SENSOR TELEMETRY SIMULATOR  v1.0             │", BOLD_FG, True)],
    [("│      Sensors: TEMP | PRES | VOLT | HRTB | RADS              │", BOLD_FG, True)],
    [("└─────────────────────────────────────────────────────────────┘", BOLD_FG, True)],
    [("        TIME │  SNS │        VALUE │    STATE", DIM_FG, False)],
    [("-------------------------------------------------------",         DIM_FG, False)],
    [("[Press Ctrl+C to stop]",                                           DIM_FG, False)],
    [],  # blank
    # readings
    [("17:55:22.844 │ TEMP │   91.221 °C   │ ", YELLOW, False), ("WARNING",  YELLOW, True)],
    [("17:55:22.844 │ PRES │   14.383 psi   │ ", GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:22.844 │ VOLT │   28.538 V     │ ", GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:22.845 │ HRTB │   56.835 bpm   │ ", YELLOW, False), ("WARNING",  YELLOW, True)],
    [("17:55:22.845 │ RADS │    0.470 μSv/h │ ", GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:23.096 │ VOLT │   27.924 V     │ ", GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:23.345 │ TEMP │   92.030 °C   │ ",  YELLOW, False), ("WARNING",  YELLOW, True)],
    [("17:55:23.597 │ PRES │   12.979 psi   │ ", YELLOW, False), ("WARNING",  YELLOW, True)],
    [("17:55:24.355 │ TEMP │   38.265 °C   │ ",  GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:24.365 │ VOLT │   28.648 V     │ ", GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:24.858 │ TEMP │   68.063 °C   │ ",  GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:25.122 │ VOLT │   26.941 V     │ ", YELLOW, False), ("WARNING",  YELLOW, True)],
    [("17:55:26.611 │ PRES │    FAULT       │ ",  MAGENTA,False), ("FAULT",   MAGENTA,True)],
    [("17:55:26.847 │ RADS │    0.514 μSv/h │ ", GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:27.363 │ PRES │    FAULT       │ ",  MAGENTA,False), ("FAULT",   MAGENTA,True)],
    [("17:55:28.883 │ TEMP │   83.394 °C   │ ",  YELLOW, False), ("WARNING",  YELLOW, True)],
    [("17:55:29.628 │ PRES │   12.608 psi   │ ", YELLOW, False), ("WARNING",  YELLOW, True)],
    [("17:55:30.197 │ VOLT │   29.419 V     │ ", GREEN,  False), ("NORMAL",   GREEN,  True)],
    [("17:55:30.955 │ VOLT │    FAULT       │ ",  MAGENTA,False), ("FAULT",   MAGENTA,True)],
    [],
    [("Shutting down...",                                                 DIM_FG, False)],
    [("Telemetry log saved to: telemetry.log",                           DIM_FG, False)],
]

def esc(s):
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

def build_svg():
    n_lines = len(LINES)
    width  = 560
    height = PAD_Y * 2 + n_lines * LINE_HEIGHT + 8

    parts = []
    parts.append(f'''<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">''')
    parts.append(f'  <rect width="{width}" height="{height}" rx="8" fill="{BG}"/>')

    # window chrome dots
    for i, color in enumerate(["#ff5f57","#febc2e","#28c840"]):
        cx = 16 + i * 20
        parts.append(f'  <circle cx="{cx}" cy="16" r="6" fill="{color}"/>')

    # title bar label
    parts.append(f'  <text x="{width//2}" y="20" text-anchor="middle" font-family="{FONT}" font-size="11" fill="{DIM_FG}">telemetry_sim</text>')

    # separator line
    sep_y = 30
    parts.append(f'  <line x1="0" y1="{sep_y}" x2="{width}" y2="{sep_y}" stroke="#313244" stroke-width="1"/>')

    # text lines
    top = sep_y + PAD_Y
    for row_i, segments in enumerate(LINES):
        y = top + row_i * LINE_HEIGHT + FONT_SIZE
        if not segments:
            continue
        x = PAD_X
        for text, color, bold in segments:
            safe = esc(text)
            weight = 'font-weight="bold"' if bold else ''
            parts.append(f'  <text x="{x}" y="{y}" font-family="{FONT}" font-size="{FONT_SIZE}" fill="{color}" {weight}>{safe}</text>')
            x += len(text) * CHAR_W

    parts.append('</svg>')
    return "\n".join(parts)

if __name__ == "__main__":
    import os
    out_dir = os.path.join(os.path.dirname(__file__), "..", "docs")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "demo.svg")
    with open(out_path, "w") as f:
        f.write(build_svg())
    print(f"Wrote {out_path}")
