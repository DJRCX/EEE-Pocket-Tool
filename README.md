# EEE Pocket Tool

Public GitHub repository: https://github.com/DJRCX/EEE-Pocket-Tool

```bash
git clone https://github.com/DJRCX/EEE-Pocket-Tool.git
cd EEE-Pocket-Tool
make
./eee_pocket_tool
```

Menu-driven C toolbox for CSE162 / EEE103IL / ECE103IL (BRAC University). It solves the five circuit utilities required by the final project brief.

```
======= EEE Pocket Tool =======
1. Series Circuit Solver
2. Parallel Circuit Solver
3. Resistor Value Determiner
4. Equivalent Resistance Calculator
5. Voltage Divider Calculator
0. Exit
```

## Build and run

You need `gcc` and `make`.

```bash
make
./eee_pocket_tool
```

Resistances are in Ohm, voltages in volts, currents in amperes. Color names are case-insensitive (`Brown`, `brown`, and `BROWN` are the same). Gold and silver are valid multipliers only.

Replay the worked examples from the lab brief:

```bash
make test
make demo
```

## What each tool does

| Choice | Tool | What you enter | What you get |
| --- | --- | --- | --- |
| 1 | Series circuit | Vs, n, R1…Rn | Req, I, voltage across each Ri |
| 2 | Parallel circuit | Vs, n, R1…Rn | Req, total I, current through each Ri |
| 3 | Color code | three band colors | resistance, including gold/silver multipliers |
| 4 | Equivalent R | series or parallel, n, values | Req only |
| 5 | Voltage divider | (a) Vin, R1, R2 or (b) Vin, R1, Vout | Vout or R2 |

Invalid input (non-positive R, unknown color, Vout ≥ Vin for a passive divider) is rejected instead of crashing.

## Lab report

The submission PDF is generated from the HTML report:

- `report/EEE103L_EEE_Pocket_Tool_Report.html` — Times New Roman 12 pt, justified body text
- `report/EEE103L_EEE_Pocket_Tool_Report.pdf` — same document, ready to submit after you fill in names
- `report/screenshots/` — console captures of every tool

Fill in the group-member table on page 1 (names, IDs, section). If you edit the HTML, rebuild:

```bash
python3 scripts/render_screenshots.py
python3 scripts/generate_report.py
google-chrome --headless --disable-gpu --no-pdf-header-footer \
  --print-to-pdf=report/EEE103L_EEE_Pocket_Tool_Report.pdf \
  "file://$PWD/report/EEE103L_EEE_Pocket_Tool_Report.html"
```

To paste the code into Google Docs with the suggested theme, copy `eee_pocket_tool.c` into a code block, choose **arduino-light**, and tick **No background**.

## Project layout

```
eee_pocket_tool.c     source (all five tools as functions)
Makefile
scripts/              demo, tests, report generators
samples/              transcripts used for screenshots
report/               HTML, PDF, screenshots
```
