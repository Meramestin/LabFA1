#!/bin/sh
set -eu
bin=${1:-./build/task_06}
unit=${2:-./build/task_06_test}
"$unit"
"$bin" > /dev/null
"$bin" 1e-6 > /dev/null
for args in '0' '-1' 'nan' '1e99999' '1e-6 extra'; do
    if "$bin" $args >/dev/null 2>&1; then exit 1; fi
done
printf 'task_06: demonstrations and input checks passed\n'

if "$bin" 1e-6 > /dev/full 2>/dev/null; then exit 1; fi
