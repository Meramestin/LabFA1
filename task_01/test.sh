#!/bin/sh
set -eu
bin=${1:-./build/task_01}
check() {
    expected=$1
    shift
    actual=$("$bin" "$@")
    test "$actual" = "$expected" || { printf 'Expected: %s\nActual: %s\n' "$expected" "$actual"; return 1; }
}
reject() {
    if "$bin" "$@" >/dev/null 2>&1; then return 1; fi
}
check '7 14 21 28 35 42 49 56 63 70 77 84 91 98' 7 -h
check 'No multiples' 101 /h
check 'No multiples' 0 -h
check 'Prime' 97 -p
check 'Composite' 100 /p
check 'Neither prime nor composite' 1 -p
check 'Neither prime nor composite' -7 -p
check '1 0 0' 256 -s
check '- F F' -255 /s
check '0' 0 -s
check '55' 10 -a
check '120' 5 -f
check '1' 0 -f
check '2432902008176640000' 20 -f
check '1 1
2 2
3 3
4 4
5 5
6 6
7 7
8 8
9 9
10 10' 1 -e
reject
reject 1
reject 1 -z
reject 1 --p
reject 1 -p extra
reject 1.0 -p
reject '' -p
reject 9223372036854775808 -p
reject -9223372036854775809 -s
reject 11 -e
reject 0 -e
reject 21 -f
reject -1 -f
reject 9223372036854775807 -a
reject -1 -a
check '25 50 75 100' -25 /h
check 'No multiples' -9223372036854775808 -h
if "$bin" 5 -f > /dev/full 2>/dev/null; then exit 1; fi
printf 'task_01: CLI and output checks passed\n'
