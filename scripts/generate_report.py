#!/usr/bin/env python3
"""Build the lab-report HTML (Times New Roman, 12pt, justified) from source + screenshots."""

from __future__ import annotations

import html
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CODE = (ROOT / "eee_pocket_tool.c").read_text(encoding="utf-8")
OUT_HTML = ROOT / "report" / "EEE103L_EEE_Pocket_Tool_Report.html"
FONT = "/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf"
FONT_B = "/usr/share/fonts/truetype/liberation/LiberationSerif-Bold.ttf"
FONT_I = "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf"
MONO = "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf"

SHOTS = [
    ("01_series.png", "Figure 1. Series Circuit Solver — Vs = 12 V, R1 = 2 Ohm, R2 = 4 Ohm."),
    ("02_parallel.png", "Figure 2. Parallel Circuit Solver — Vs = 12 V, R1 = 6 Ohm, R2 = 3 Ohm."),
    ("03_color_code.png", "Figure 3. Resistor Value Determiner — brown, black, orange (lab example = 10 kOhm)."),
    ("04_equivalent_series.png", "Figure 4. Equivalent Resistance Calculator — series, 10 + 20 + 30 Ohm."),
    ("04_equivalent_parallel.png", "Figure 5. Equivalent Resistance Calculator — parallel, 10 || 10 Ohm."),
    ("05a_voltage_divider_vout.png", "Figure 6. Voltage divider (a) — Vin = 12 V, R1 = 1 kOhm, R2 = 2 kOhm."),
    ("05b_voltage_divider_r2.png", "Figure 7. Voltage divider (b) — Vin = 12 V, R1 = 1 kOhm, Vout = 8 V."),
]


def figure(filename: str, caption: str) -> str:
    src = (ROOT / "report" / "screenshots" / filename).resolve().as_uri()
    return f"""
<figure>
  <img src=\"{src}\" alt=\"{html.escape(caption)}\">
  <figcaption>{html.escape(caption)}</figcaption>
</figure>
"""


def main() -> None:
    figures = "\n".join(figure(name, cap) for name, cap in SHOTS)
    code = html.escape(CODE)

    doc = f"""<!DOCTYPE html>
<html lang=\"en\">
<head>
<meta charset=\"utf-8\">
<title>EEE Pocket Tool – Final Project Report</title>
<style>
@font-face {{
  font-family: \"Times New Roman\";
  src: url(\"{Path(FONT).resolve().as_uri()}\") format(\"truetype\");
  font-weight: 400;
  font-style: normal;
}}
@font-face {{
  font-family: \"Times New Roman\";
  src: url(\"{Path(FONT_B).resolve().as_uri()}\") format(\"truetype\");
  font-weight: 700;
  font-style: normal;
}}
@font-face {{
  font-family: \"Times New Roman\";
  src: url(\"{Path(FONT_I).resolve().as_uri()}\") format(\"truetype\");
  font-weight: 400;
  font-style: italic;
}}
@font-face {{
  font-family: \"Courier New\";
  src: url(\"{Path(MONO).resolve().as_uri()}\") format(\"truetype\");
  font-weight: 400;
  font-style: normal;
}}
@page {{
  size: A4;
  margin: 25mm 25mm 25mm 25mm;
}}
html, body {{
  font-family: \"Times New Roman\", Times, serif;
  font-size: 12pt;
  line-height: 1.5;
  color: #111;
  text-align: justify;
}}
h1, h2, h3 {{
  text-align: center;
  font-weight: 700;
  page-break-after: avoid;
}}
h1 {{ font-size: 16pt; margin: 0.4em 0; }}
h2 {{ font-size: 14pt; margin-top: 1.4em; text-align: left; }}
h3 {{ font-size: 12pt; margin-top: 1.1em; text-align: left; }}
.cover {{ text-align: center; margin-bottom: 2em; }}
.cover p {{ text-align: center; margin: 0.25em 0; }}
table.meta {{
  margin: 1.2em auto 0;
  border-collapse: collapse;
  font-size: 12pt;
}}
table.meta td {{
  text-align: left;
  padding: 4px 18px;
  vertical-align: top;
}}
p {{ margin: 0.55em 0; }}
ul, ol {{ margin: 0.4em 0 0.4em 1.4em; }}
li {{ margin: 0.2em 0; }}
code, pre {{
  font-family: \"Courier New\", Courier, monospace;
  font-size: 8.5pt;
  text-align: left;
}}
pre.source {{
  background: #fafafa;
  border: 1px solid #ddd;
  padding: 10px 12px;
  white-space: pre;
  overflow-wrap: normal;
  line-height: 1.35;
  page-break-inside: auto;
}}
figure {{
  margin: 1.1em 0;
  text-align: center;
  page-break-inside: avoid;
}}
figure img {{
  max-width: 100%;
  height: auto;
  border: 1px solid #ccc;
}}
figcaption {{
  font-size: 11pt;
  font-style: italic;
  text-align: center;
  margin-top: 0.35em;
}}
.eq {{
  text-align: center;
  font-style: italic;
  margin: 0.6em 0;
}}
.fill {{
  border-bottom: 1px solid #333;
  display: inline-block;
  min-width: 220px;
}}
</style>
</head>
<body>

<div class=\"cover\">
  <p><strong>BRAC University</strong></p>
  <p>Department of Electrical and Electronic Engineering</p>
  <p>CSE162 / EEE103IL / ECE103IL: Computer Programming Laboratory</p>
  <p><strong>Final Project Report</strong></p>
  <h1>EEE Pocket Tool – A Multi-Functional Circuit Assistant in C</h1>
  <table class=\"meta\">
    <tr><td>Submitted by (group)</td><td><span class=\"fill\">&nbsp;</span></td></tr>
    <tr><td>Student names and IDs</td><td><span class=\"fill\">&nbsp;</span></td></tr>
    <tr><td></td><td><span class=\"fill\">&nbsp;</span></td></tr>
    <tr><td>Section / Faculty</td><td><span class=\"fill\">&nbsp;</span></td></tr>
    <tr><td>Date</td><td>4 September 2026</td></tr>
  </table>
</div>

<h2>1. Objectives</h2>
<p>The objective of this project is to design and implement a user-friendly, console-based C application that acts as a pocket toolbox for common first-year circuit calculations. The program must present a clear menu, collect the inputs required by each tool, and print the electrical results in a structured, readable format. Specifically, the software must provide:</p>
<ul>
  <li>a series circuit solver for one voltage source and n resistors;</li>
  <li>a parallel circuit solver for one voltage source and n resistors;</li>
  <li>a resistor value determiner based on the 4-band color code (first three bands);</li>
  <li>an equivalent resistance calculator for a series or parallel set of resistors;</li>
  <li>a voltage divider calculator that can either find V<sub>out</sub> or design R<sub>2</sub> for a desired V<sub>out</sub>.</li>
</ul>
<p>A further objective is to organise the program so that each tool is a dedicated function, to validate user input, and to document the work with comments and sample runs.</p>

<h2>2. Introduction</h2>
<p>Students and engineers frequently need quick numerical checks while breadboarding, sitting in a lab viva, or estimating a divider ratio before picking parts. A dedicated calculator that speaks the language of circuits — equivalent resistance, branch current, color codes — is more convenient than a general scientific calculator, because the formulas and the prompts are already in place. This project implements that idea as a single C program. The menu follows the laboratory brief; extra robustness (rejection of non-positive resistances, unknown color names, and impossible divider ratios) was added so that a mistyped value does not produce a meaningless or crashing result.</p>

<h2>3. Theory and Equations</h2>
<h3>3.1 Series circuit</h3>
<p>Resistors in series share the same current. The equivalent resistance is the arithmetic sum of the individual values. Ohm’s law then gives the loop current, and each voltage drop follows from that current:</p>
<p class=\"eq\">R<sub>eq</sub> = R<sub>1</sub> + R<sub>2</sub> + · · · + R<sub>n</sub></p>
<p class=\"eq\">I = V<sub>s</sub> / R<sub>eq</sub></p>
<p class=\"eq\">V<sub>i</sub> = I · R<sub>i</sub></p>
<p>Kirchhoff’s voltage law requires that the sum of the resistor drops equal the source voltage. The program prints this sum as a check.</p>

<h3>3.2 Parallel circuit</h3>
<p>Resistors in parallel share the same voltage. The reciprocal of the equivalent resistance is the sum of the reciprocals. Total current is V<sub>s</sub> / R<sub>eq</sub>, and each branch current is V<sub>s</sub> / R<sub>i</sub>:</p>
<p class=\"eq\">1 / R<sub>eq</sub> = Σ (1 / R<sub>i</sub>)</p>
<p class=\"eq\">I = V<sub>s</sub> / R<sub>eq</sub></p>
<p class=\"eq\">I<sub>i</sub> = V<sub>s</sub> / R<sub>i</sub></p>
<p>Kirchhoff’s current law requires that the branch currents add up to the total current. That sum is also printed as a check.</p>

<h3>3.3 Four-band resistor color code</h3>
<p>The first two bands are significant digits. The third band is a multiplier. With Black at index 0, Brown at 1, and so on through White at 9, the resistance is</p>
<p class=\"eq\">R = (d<sub>1</sub> × 10 + d<sub>2</sub>) × 10<sup>d<sub>3</sub></sup></p>
<p>Gold and silver are not digits. They are special multipliers of 0.1 and 0.01 respectively, so they are handled with a separate comparison rather than as array indices. The laboratory example is brown–black–orange: (1 × 10 + 0) × 1000 = 10 000 Ohm.</p>

<h3>3.4 Equivalent resistance</h3>
<p>This tool uses the same series and parallel formulas as Sections 3.1 and 3.2, but it does not require a source voltage. The user chooses the connection type, enters n and the resistor values, and receives only R<sub>eq</sub>.</p>

<h3>3.5 Voltage divider</h3>
<p>For the unloaded divider of R<sub>1</sub> (from Vin to the output node) and R<sub>2</sub> (from the output node to ground),</p>
<p class=\"eq\">V<sub>out</sub> = V<sub>in</sub> · R<sub>2</sub> / (R<sub>1</sub> + R<sub>2</sub>)</p>
<p>Solving for R<sub>2</sub> when R<sub>1</sub>, V<sub>in</sub> and a desired V<sub>out</sub> are known gives</p>
<p class=\"eq\">R<sub>2</sub> = R<sub>1</sub> · V<sub>out</sub> / (V<sub>in</sub> − V<sub>out</sub>)</p>
<p>A passive divider cannot produce V<sub>out</sub> ≥ V<sub>in</sub>. The program rejects that case instead of dividing by zero or reporting a negative resistor.</p>

<h2>4. Implementation</h2>
<p>The program is a single C source file compiled with gcc against the math library. After a short welcome message, <code>main</code> prints the menu in a loop and dispatches on the user’s choice:</p>
<ol>
  <li><code>series_circuit_solver</code></li>
  <li><code>parallel_circuit_solver</code></li>
  <li><code>resistor_value_determiner</code></li>
  <li><code>equivalent_resistance_calculator</code></li>
  <li><code>voltage_divider_calculator</code> (submenu for V<sub>out</sub> or R<sub>2</sub>)</li>
  <li>choice 0 exits</li>
</ol>
<p>Resistor lists are stored in a double array of length 32 and processed with <code>for</code> loops, matching the laboratory hint. Color names are read as strings, trimmed, converted to lowercase, and compared with <code>strcmp</code>. Digit colors occupy indices 0–9 of a name table, so the index is the digit. The multiplier is 10 raised to that digit, except for gold and silver.</p>
<p>All numeric input is read as a full line and parsed with <code>sscanf</code>. This avoids leftover-newline bugs that appear when <code>scanf</code> is mixed with string input. Values that are non-numeric, non-positive (for R and V<sub>s</sub>), or out of the allowed menu range are rejected with a short message and the prompt is repeated.</p>

<h2>5. Full Source Code</h2>
<p>The complete program is listed below. Comments describe the reasoning behind each tool rather than restating the obvious syntax of C.</p>
<pre class=\"source\">{code}</pre>

<h2>6. Sample Outputs</h2>
<p>Each of the five required tools was executed at least once. The figures below are console transcripts of those runs. Typed inputs are shown on the same line as the prompt, as they appear in an interactive session.</p>
{figures}

<h2>7. Discussion</h2>
<p>The numerical results match hand calculation. For the series example, 2 Ohm and 4 Ohm give R<sub>eq</sub> = 6 Ohm and I = 12 / 6 = 2 A, so the drops are 4 V and 8 V and they sum to the source. For the parallel example, 6 Ohm || 3 Ohm is 2 Ohm, the total current is 6 A, and the branches draw 2 A and 4 A. The color-code example reproduces the value given in the project brief (10 kOhm). The equivalent-resistance tool is consistent with the first two solvers: 10 + 20 + 30 = 60 Ohm in series and 10 || 10 = 5 Ohm in parallel. The divider examples are inverses of each other: 12 V, 1 kOhm and 2 kOhm produce 8 V, and asking for 8 V with the same V<sub>in</sub> and R<sub>1</sub> returns R<sub>2</sub> = 2 kOhm.</p>
<p>Gold as a multiplier was also checked (red–red–gold = 2.2 Ohm) to confirm that the special cases described in the hint are not treated as powers of ten. Input validation was exercised during development: unknown colors, V<sub>out</sub> ≥ V<sub>in</sub>, and non-positive resistances are refused.</p>
<p>A practical limitation is that the solvers treat only a single source and a pure series or pure parallel network. Mixed series–parallel ladders, current sources, and five-band color codes were left out so that the required tools stayed clear and testable. Those would be natural extensions.</p>

<h2>8. Conclusion</h2>
<p>A menu-driven C program named EEE Pocket Tool was implemented with five circuit utilities, each isolated in its own function. Arrays and loops compute series and parallel networks; string comparison decodes resistor color bands; and the voltage-divider pair both analyses and designs a two-resistor divider. Sample runs confirm that the printed results agree with the governing equations. The program is therefore a complete solution to the laboratory project as specified.</p>

<h2>9. How to Compile and Run</h2>
<p>On a system with gcc installed:</p>
<pre class=\"source\">make
./eee_pocket_tool</pre>
<p>The optional target <code>make test</code> replays the sample cases used in Section 6 and checks the printed results.</p>

<p style=\"margin-top:2em; text-align:center;\"><em>End of report</em></p>

</body>
</html>
"""
    OUT_HTML.parent.mkdir(parents=True, exist_ok=True)
    OUT_HTML.write_text(doc, encoding=\"utf-8\")
    print(f\"wrote {OUT_HTML}\")


if __name__ == \"__main__\":
    main()
