#!/bin/sh
set -eu
bin=$(realpath "${1:-./build/task_07}")
dir=$(mktemp -d)
trap 'rm -rf "$dir"' EXIT
printf 'a\tb\nc d' > "$dir/one"
printf '1  2' > "$dir/two"
"$bin" -r "$dir/one" "$dir/two" "$dir/out"
test "$(cat "$dir/out")" = 'a 1 b 2 c d'
"$bin" /r "$dir/two" "$dir/one" "$dir/out"
test "$(cat "$dir/out")" = '1 a 2 b c d'
: > "$dir/empty"
"$bin" -r "$dir/empty" "$dir/one" "$dir/out"
test "$(cat "$dir/out")" = 'a b c d'
"$bin" -r "$dir/empty" "$dir/empty" "$dir/out"
test ! -s "$dir/out"
printf 'A B C D E F G H I J K L M N O P Q R S T' > "$dir/letters"
"$bin" -a "$dir/letters" "$dir/out"
test "$(cat "$dir/out")" = 'A b C d 105 f G h I 1222 K l M n 117 p Q r S 1310'
"$bin" /a "$dir/empty" "$dir/out"
test ! -s "$dir/out"
for bad in "$dir/one" "$dir/two"; do
    if "$bin" -r "$dir/one" "$dir/two" "$bad" 2>/dev/null; then exit 1; fi
done
if "$bin" -a "$dir/letters" "$dir/letters" 2>/dev/null; then exit 1; fi
if "$bin" -r "$dir/one" "$dir/missing" "$dir/out" 2>/dev/null; then exit 1; fi
if "$bin" -r "$dir/one" "$dir/out" 2>/dev/null; then exit 1; fi
if "$bin" -a "$dir/one" "$dir/two" "$dir/out" 2>/dev/null; then exit 1; fi
if "$bin" -z "$dir/one" "$dir/out" 2>/dev/null; then exit 1; fi
printf 'task_07: merge, token transformations and errors passed\n'

printf 'ab\000cd' > "$dir/nul"
if "$bin" -a "$dir/nul" "$dir/out" 2>/dev/null; then exit 1; fi
