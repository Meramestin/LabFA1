#!/bin/sh
set -eu
bin=$(realpath "${1:-./build/task_08}")
dir=$(mktemp -d)
trap 'rm -rf "$dir"' EXIT
printf '000 00101\tff Z\n-0010 +000a -0' > "$dir/in"
"$bin" "$dir/in" "$dir/out"
printf '0 2 0\n101 2 5\nFF 16 255\nZ 36 35\n-10 2 -2\nA 11 10\n0 2 0\n' > "$dir/expected"
cmp "$dir/out" "$dir/expected"
: > "$dir/empty"
"$bin" "$dir/empty" "$dir/out"
test ! -s "$dir/out"
for bad in '12?' '+' '-' '1.0' 'ZZZZZZZZZZZZZZZZZZ'; do
    printf '%s' "$bad" > "$dir/bad"
    if "$bin" "$dir/bad" "$dir/out" 2>/dev/null; then exit 1; fi
done
cp "$dir/in" "$dir/copy"
if "$bin" "$dir/in" "$dir/in" 2>/dev/null; then exit 1; fi
cmp "$dir/in" "$dir/copy"
if "$bin" "$dir/missing" "$dir/out" 2>/dev/null; then exit 1; fi
if "$bin" 2>/dev/null; then exit 1; fi
if "$bin" "$dir/in" "$dir/out" extra 2>/dev/null; then exit 1; fi
printf 'task_08: base conversion and error checks passed\n'

printf 'ab\000cd' > "$dir/nul"
if "$bin" "$dir/nul" "$dir/out" 2>/dev/null; then exit 1; fi
