#!/bin/sh
set -eu
bin=${1:-./build/task_06}
unit=${2:-./build/task_06_test}
"$unit"
"$bin" > /dev/null
"$bin" 1e-6 > /dev/null
"$bin" 1e-12 | python3 -c '
from decimal import Decimal, getcontext
import sys
getcontext().prec = 50
lines = dict(line.strip().split(": ", 1) for line in sys.stdin)
assert abs(Decimal(lines["root x*x-2"]) - Decimal(2).sqrt()) <= Decimal("1e-12")
assert abs(Decimal(lines["root cos(x)-x"]) - Decimal("0.7390851332151606416553120876738734040134")) <= Decimal("1e-11")
'
for args in '0' '-1' 'nan' '1e99999' '1e400' '1e-6 extra'; do
    if "$bin" $args >/dev/null 2>&1; then exit 1; fi
done
printf 'task_06: demonstrations and input checks passed\n'

if "$bin" 1e-6 > /dev/full 2>/dev/null; then exit 1; fi
