#!/bin/sh
set -eu
bin=${1:-./build/task_10}
actual=$(printf '16\n000FF -1 0 Stop\n' | "$bin")
test "$actual" = 'max 16 FF
sum 16 FE
max 9 313
sum 9 312
max 18 E3
sum 18 E2
max 27 9C
sum 27 9B
max 36 73
sum 36 72'
actual=$(printf '10 -10 3 Stop' | "$bin")
test "$(printf '%s\n' "$actual" | head -2)" = 'max 10 -10
sum 10 -7'
actual=$(printf '2 000 -0 Stop' | "$bin")
test "$(printf '%s\n' "$actual" | head -2)" = 'max 2 0
sum 2 0'
printf '10 -9223372036854775808 Stop' | "$bin" > /dev/null
printf '36 Z Stop' | "$bin" > /dev/null
for input in '1 0 Stop' '37 0 Stop' '16 ff Stop' '2 2 Stop' '10 Stop' '10 1' '10 9223372036854775807 1 Stop' '10 -9223372036854775808 -1 Stop' '10 9223372036854775808 Stop' '10 + Stop' ''; do
    if printf '%s' "$input" | "$bin" >/dev/null 2>&1; then exit 1; fi
done
if "$bin" extra </dev/null >/dev/null 2>&1; then exit 1; fi
printf 'task_10: input, arithmetic boundaries and bases passed\n'

if printf "10 7 Stop" | "$bin" > /dev/full 2>/dev/null; then exit 1; fi
