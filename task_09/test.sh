#!/bin/sh
set -eu
bin=${1:-./build/task_09}
unit=${2:-./build/task_09_test}
"$unit"
"$bin" 0 0 > /dev/null
"$bin" -100 100 > /dev/null
"$bin" -2147483648 2147483647 > /dev/null
for args in '2 1' '0' '0 1 2' 'a 2' '0 2147483648'; do
    if "$bin" $args >/dev/null 2>&1; then exit 1; fi
done
if "$bin" >/dev/null 2>&1; then exit 1; fi
printf 'task_09: random runs and argument checks passed\n'

if "$bin" 0 0 > /dev/full 2>/dev/null; then exit 1; fi
