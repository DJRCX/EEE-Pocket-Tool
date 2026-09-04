#!/usr/bin/env bash
# Numeric sanity checks against the lab's worked examples.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/eee_pocket_tool"

if [[ ! -x "$BIN" ]]; then
  echo "Build the program first: make" >&2
  exit 1
fi

fail=0
check() {
  local name="$1" needle="$2" output="$3"
  if grep -F -q "$needle" <<<"$output"; then
    echo "PASS  $name"
  else
    echo "FAIL  $name"
    echo "  expected to find: $needle"
    echo "$output" | sed 's/^/  | /'
    fail=1
  fi
}

# 1. Series: Vs=12, R=2+4 -> Req=6 Ohm, I=2 A, V=4 V and 8 V
out="$("$BIN" <<'EOF'
1
12
2
2
4

0
EOF
)"
check "series Req" "Equivalent resistance Req : 6 Ohm" "$out"
check "series I"   "Series current I          : 2 A" "$out"
check "series V1"  "4 V" "$out"
check "series V2"  "8 V" "$out"

# 2. Parallel: Vs=12, R=6 || 3 -> Req=2 Ohm, I=6 A, I1=2 A, I2=4 A
out="$("$BIN" <<'EOF'
2
12
2
6
3

0
EOF
)"
check "parallel Req" "Equivalent resistance Req : 2 Ohm" "$out"
check "parallel I"   "Total current I           : 6 A" "$out"

# 3. Color: brown-black-orange = 10000 Ohm (lab example)
out="$("$BIN" <<'EOF'
3
brown
black
orange

0
EOF
)"
check "color 10k" "10 kOhm" "$out"
check "color calc" "(1 x 10 + 0) x 1000" "$out"

# 4. Equivalent series 10+20+30 = 60; parallel 10||10 = 5
out="$("$BIN" <<'EOF'
4
s
3
10
20
30

0
EOF
)"
check "eq series" "Req           : 60 Ohm" "$out"

out="$("$BIN" <<'EOF'
4
p
2
10
10

0
EOF
)"
check "eq parallel" "Req           : 5 Ohm" "$out"

# 5a. Vin=12, R1=1k, R2=2k -> Vout=8 V
out="$("$BIN" <<'EOF'
5
1
12
1000
2000

0
EOF
)"
check "divider Vout" "Vout : 8 V" "$out"

# 5b. Vin=12, R1=1k, Vout=8 -> R2=2 kOhm
out="$("$BIN" <<'EOF'
5
2
12
1000
8

0
EOF
)"
check "divider R2" "R2   : 2 kOhm" "$out"

# Gold multiplier: red-red-gold = 2.2 Ohm
out="$("$BIN" <<'EOF'
3
red
red
gold

0
EOF
)"
check "gold multiplier" "2.2 Ohm" "$out"

if [[ "$fail" -ne 0 ]]; then
  echo "Some checks failed."
  exit 1
fi
echo "All checks passed."
