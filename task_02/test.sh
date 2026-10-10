#!/bin/sh
set -eu
bin=${1:-./build/task_02}
unit=${2:-./build/task_02_test}
"$unit"
test "$("$bin" 1e-5 | wc -l)" -eq 5
for args in '' '0' '-1' 'nan' 'inf' '1e99999' '1e-99999' '1e-30' '1e-6 extra'; do
    if "$bin" $args >/dev/null 2>&1; then exit 1; fi
done
printf 'task_02: demonstrations and argument checks passed\n'

if "$bin" 1e-4 > /dev/full 2>/dev/null; then exit 1; fi
