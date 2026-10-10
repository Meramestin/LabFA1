#!/bin/sh
set -eu
bin=$(realpath "${1:-./build/task_04}")
dir=$(mktemp -d)
trap 'rm -rf "$dir"' EXIT
printf 'Ab12 !\nZ9\t?\nend' > "$dir/input.txt"
"$bin" -d "$dir/input.txt"
printf 'Ab !\nZ\t?\nend' > "$dir/expected"
cmp "$dir/out_input.txt" "$dir/expected"
"$bin" /ni "$dir/input.txt" "$dir/result"
printf '2\n1\n3\n' > "$dir/expected"
cmp "$dir/result" "$dir/expected"
"$bin" -ns "$dir/input.txt" "$dir/result"
printf '1\n2\n0\n' > "$dir/expected"
cmp "$dir/result" "$dir/expected"
printf 'A1 z\n' > "$dir/small"
"$bin" -na "$dir/small" "$dir/result"
test "$(cat "$dir/result")" = '411207AA'
printf '\n\n' > "$dir/blank"
"$bin" -ni "$dir/blank" "$dir/result"
printf '0\n0\n' > "$dir/expected"
cmp "$dir/result" "$dir/expected"
: > "$dir/empty"
"$bin" -ni "$dir/empty" "$dir/result"
test ! -s "$dir/result"
cp "$dir/input.txt" "$dir/copy"
ln "$dir/input.txt" "$dir/link"
ln -s "$dir/input.txt" "$dir/symlink"
for output in "$dir/input.txt" "$dir/link" "$dir/symlink"; do
    if "$bin" -nd "$dir/input.txt" "$output" 2>/dev/null; then exit 1; fi
    cmp "$dir/input.txt" "$dir/copy"
done
if "$bin" -d "$dir/missing" 2>/dev/null; then exit 1; fi
if "$bin" -nd "$dir/input.txt" "$dir/missing/result" 2>/dev/null; then exit 1; fi
if "$bin" -d "$dir/input.txt" extra 2>/dev/null; then exit 1; fi
if "$bin" -nz "$dir/input.txt" "$dir/result" 2>/dev/null; then exit 1; fi
if "$bin" 2>/dev/null; then exit 1; fi
printf 'task_04: file transformations and error checks passed\n'

printf 'ab\000cd' > "$dir/nul"
"$bin" -nd "$dir/nul" "$dir/result"
cmp "$dir/nul" "$dir/result"
