#!/bin/sh
# usage: sh tests/run_tests.sh [binary]   (default ./codexion)
BIN=${1:-./codexion}
fail=0
run() {
	out=$($BIN "$@" 2>&1)
	res=$(printf '%s\n' "$out" | python3 tests/check_log.py "$@" 2>&1 | tail -1)
	printf '%-42s %s\n' "$*" "$res"
	[ "$res" = "OK" ] || fail=1
}
for pol in fifo edf; do
	run 2 500 100 50 50 5 0 $pol
	run 3 700 100 50 50 5 0 $pol
	run 4 800 150 100 100 6 0 $pol
	run 5 800 200 200 200 5 0 $pol
	run 5 900 100 100 100 8 20 $pol
	run 8 1000 100 100 100 6 10 $pol
	run 50 1500 50 50 50 5 5 $pol
	run 200 4000 50 50 50 3 0 $pol
	run 1 300 100 100 100 3 0 $pol
	run 4 410 200 200 200 10 0 $pol
	run 2 150 100 100 100 5 0 $pol
done
exit $fail
