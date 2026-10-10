#!/bin/sh
set -eu
bin=${1:-./build/task_03}
unit=${2:-./build/task_03_test}
"$unit"
test "$("$bin" -m -12 3)" = 'Multiple'
test "$("$bin" /m 12 5)" = 'Not multiple'
test "$("$bin" -t 0.000001 3 4 5)" = 'Right triangle'
test "$("$bin" -t 0.000001 3 4 6)" = 'Not right triangle'
test "$("$bin" -q 0.000001 1 2 3 | wc -l)" -eq 6
test "$("$bin" -q 0.000001 1 1 2 | wc -l)" -eq 3
test "$("$bin" /q 0.000001 1 1 1 | wc -l)" -eq 1
"$bin" -q 1e-14 1 0 -2 | python3 -c '
from decimal import Decimal, getcontext
import sys
getcontext().prec = 50
line = sys.stdin.readline()
roots = [Decimal(text) for text in line.split(":")[1].split()]
expected = Decimal(2).sqrt()
assert len(roots) == 2
assert all(abs(abs(root) - expected) <= Decimal("1e-14") for root in roots)
'
for args in '-m 0 1' '-m 1 0' '-m 1' '-m 1 2 3' '-q 0 1 2 3' '-t -1 3 4 5' '-t 1e-6 0 4 5' '-q nan 1 2 3' '-q 1e-99999 1 2 3' '-q 1e-6 1x 2 3' '-z 1 2'; do
    if "$bin" $args >/dev/null 2>&1; then exit 1; fi
done
if "$bin" >/dev/null 2>&1; then exit 1; fi
printf 'task_03: CLI and unique permutations passed\n'

if "$bin" -m 12 3 > /dev/full 2>/dev/null; then exit 1; fi
