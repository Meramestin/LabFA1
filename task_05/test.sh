#!/bin/sh
set -eu
bin=${1:-./build/task_05}
unit=${2:-./build/task_05_test}
"$unit"
"$bin" 1e-5 0.5 > /dev/null
"$bin" 1e-5 0 > /dev/null
"$bin" 1e-5 -0.5 > /dev/null
"$bin" 1e-9 0.5 > /dev/null
for args in '' '1e-6' '0 0.5' '-1 0.5' 'nan 0.5' '1e-6 1' '1e-6 1.1' '1e-99999 0.5' '1e-6 0.5 extra'; do
    if "$bin" $args >/dev/null 2>&1; then exit 1; fi
done
printf 'task_05: CLI checks passed\n'

if "$bin" 1e-5 0.5 > /dev/full 2>/dev/null; then exit 1; fi
