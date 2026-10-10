#!/usr/bin/env python3
"""Compile C99 applications and run their existing tests from any directory."""
import argparse
import os
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--sanitize', action='store_true',
                        help='enable AddressSanitizer and UndefinedBehaviorSanitizer')
    parser.add_argument('--no-leaks', action='store_true',
                        help='disable LeakSanitizer when the environment prevents its use')
    options = parser.parse_args()
    if options.no_leaks and not options.sanitize:
        parser.error('--no-leaks requires --sanitize')
    environment = os.environ.copy()
    if options.no_leaks:
        environment['ASAN_OPTIONS'] = environment.get('ASAN_OPTIONS', '') + ':detect_leaks=0'
        print('Leak detection is disabled; check leaks separately with Valgrind.', flush=True)
    root = Path(__file__).resolve().parent
    build = root / ('build_sanitize' if options.sanitize else 'build')
    build.mkdir(exist_ok=True)
    flags = ['-std=c99', '-Wall', '-Wextra', '-Wpedantic', '-Werror', '-g']
    if options.sanitize:
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
    tasks = sorted(root.glob('task_[0-9][0-9]'))
    for task in tasks:
        for source in sorted(task.glob('*.c')):
            suffix = '_test' if source.name == 'test.c' else ''
            subprocess.run(['gcc', *flags, str(source), '-lm', '-o',
                            str(build / (task.name + suffix))], check=True)
    for task in tasks:
        command = ['sh', str(task / 'test.sh'), str(build / task.name)]
        if (task / 'test.c').exists():
            command.append(str(build / (task.name + '_test')))
        subprocess.run(command, check=True, cwd=root, env=environment)
    print('All 10 tasks passed' + (' with sanitizers.' if options.sanitize else '.'))


if __name__ == '__main__':
    main()
