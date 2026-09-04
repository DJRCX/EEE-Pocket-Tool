#!/usr/bin/env bash
# Feed representative inputs through every tool and capture the session.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/eee_pocket_tool"
OUT_DIR="$ROOT/samples"
mkdir -p "$OUT_DIR"

if [[ ! -x "$BIN" ]]; then
  echo "Build the program first: make" >&2
  exit 1
fi

# After each tool the program waits for Enter (empty line) before the menu.
"$BIN" > "$OUT_DIR/full_session.txt" <<'EOF'
1
12
2
2
4

2
12
2
6
3

3
brown
black
orange

4
s
3
10
20
30

4
p
2
10
10

5
1
12
1000
2000

5
2
12
1000
8

3
red
red
gold

0
EOF

echo "Wrote $OUT_DIR/full_session.txt"
