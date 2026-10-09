#!/bin/sh
# Full regression test suite for the simplified ls project.
# Run from project root: sh test_full.sh
# Intended for NetBSD /bin/sh.

PASS=0
FAIL=0
WARN=0
TMPDIR_TEST="${TMPDIR:-/tmp}/ls_test_$$"
GREEN=''
RED=''
YELLOW=''
NC=''

if [ -t 1 ]; then
    GREEN=$(printf '\033[0;32m')
    RED=$(printf '\033[0;31m')
    YELLOW=$(printf '\033[1;33m')
    NC=$(printf '\033[0m')
fi

cleanup() { rm -rf "$TMPDIR_TEST"; }
trap cleanup EXIT HUP INT TERM
mkdir -p "$TMPDIR_TEST" || exit 1

echo "======================================"
echo "FULL TEST SUITE - SIMPLIFIED LS"
echo "======================================"
echo

if [ ! -x ./ls ]; then
    echo "Binary ./ls not found; trying to build..."
    if ! make; then
        echo "Build failed. Stop testing."
        exit 1
    fi
fi
[ -x ./ls ] || { echo "ERROR: ./ls is not executable."; exit 1; }

run_test() {
    label=$1
    shift
    printf 'Test: %s\n' "$label"
    if "$@" >"$TMPDIR_TEST/stdout" 2>"$TMPDIR_TEST/stderr"; then
        printf '%sPASS%s\n\n' "$GREEN" "$NC"
        PASS=$((PASS + 1))
    else
        status=$?
        printf '%sFAIL%s (exit code %s)\n' "$RED" "$NC" "$status"
        [ ! -s "$TMPDIR_TEST/stderr" ] || cat "$TMPDIR_TEST/stderr"
        echo
        FAIL=$((FAIL + 1))
    fi
}

run_error_test() {
    label=$1
    shift
    printf 'Test: %s\n' "$label"
    if "$@" >"$TMPDIR_TEST/stdout" 2>"$TMPDIR_TEST/stderr"; then
        printf '%sFAIL%s (expected non-zero exit)\n\n' "$RED" "$NC"
        FAIL=$((FAIL + 1))
    else
        printf '%sPASS%s (error exit as expected)\n\n' "$GREEN" "$NC"
        PASS=$((PASS + 1))
    fi
}

mkdir -p "$TMPDIR_TEST/fixture/subdir" "$TMPDIR_TEST/empty"
: > "$TMPDIR_TEST/fixture/ordinary.txt"
: > "$TMPDIR_TEST/fixture/.hidden"
printf 'sample data\n' > "$TMPDIR_TEST/fixture/medium.txt"
chmod 755 "$TMPDIR_TEST/fixture/medium.txt"
ln -s ordinary.txt "$TMPDIR_TEST/fixture/file-link" 2>/dev/null || :
ln -s subdir "$TMPDIR_TEST/fixture/dir-link" 2>/dev/null || :
if command -v mkfifo >/dev/null 2>&1; then
    mkfifo "$TMPDIR_TEST/fixture/pipe" 2>/dev/null || :
fi

echo "=== 1. BASIC OPTIONS ==="
run_test "No arguments" ./ls
run_test "-l long format" ./ls -l
run_test "-a includes . and .." ./ls -a
run_test "-A excludes . and .." ./ls -A
run_test "-h human-readable size" ./ls -h
run_test "-i inode" ./ls -i
run_test "-s block count" ./ls -s
run_test "-k kilobytes with blocks" ./ls -s -k
run_test "-t time sort" ./ls -t
run_test "-S size sort" ./ls -S
run_test "-r reverse sort" ./ls -r
run_test "-R recursive listing" ./ls -R src
run_test "-d directory as file" ./ls -d .
run_test "-F file type indicators" ./ls -F
run_test "-f no sort" ./ls -f
run_test "-n numeric UID/GID and long format" ./ls -n
run_test "-c ctime" ./ls -l -c
run_test "-u atime" ./ls -l -u
run_test "-q non-printable handling" ./ls -q
run_test "-w raw filename handling" ./ls -w

echo
echo "=== 2. SEPARATE OPTION COMBINATIONS ==="
# This parser accepts separate options, not combined forms such as -la.
run_test "Long + all" ./ls -l -a
run_test "Long + human-readable" ./ls -l -h
run_test "Long + time sort" ./ls -l -t
run_test "Long + size sort" ./ls -l -S
run_test "All + recursive" ./ls -a -R src
run_test "Long + inode + blocks" ./ls -l -i -s

echo
echo "=== 3. OPTION PRECEDENCE ==="
run_test "-l then -n (numeric mode)" ./ls -l -n
run_test "-n then -l (name mode)" ./ls -n -l
run_test "-c then -u (atime mode)" ./ls -l -c -u
run_test "-u then -c (ctime mode)" ./ls -l -u -c
run_test "-R then -d (directory mode)" ./ls -R -d .
run_test "-d then -R (recursive mode)" ./ls -d -R src
run_test "-k then -h (human-readable size)" ./ls -s -k -h
run_test "-h then -k (kilobytes)" ./ls -s -h -k
run_test "-q then -w (raw mode)" ./ls -q -w
run_test "-w then -q (quoted mode)" ./ls -w -q

echo
echo "=== 4. PATHS AND MULTIPLE OPERANDS ==="
run_test "List Makefile and README.md" ./ls Makefile README.md
run_test "List src and include" ./ls src include
run_test "List directory operand as file" ./ls -d src
run_test "List home directory" ./ls "$HOME"
run_test "List /tmp" ./ls -a /tmp

echo
echo "=== 5. FIXTURE / EDGE CASES ==="
run_test "List fixture directory" ./ls "$TMPDIR_TEST/fixture"
run_test "Hidden files in fixture" ./ls -a "$TMPDIR_TEST/fixture"
run_test "Classify fixture entries" ./ls -F "$TMPDIR_TEST/fixture"
run_test "Long listing fixture" ./ls -l "$TMPDIR_TEST/fixture"
run_test "Recursive fixture" ./ls -R "$TMPDIR_TEST/fixture"
run_test "Empty directory" ./ls "$TMPDIR_TEST/empty"
run_error_test "Non-existent path" ./ls "$TMPDIR_TEST/does-not-exist"

if [ -L "$TMPDIR_TEST/fixture/file-link" ]; then
    run_test "Symbolic link listing" ./ls -l "$TMPDIR_TEST/fixture/file-link"
    run_test "Symbolic link classification" ./ls -F "$TMPDIR_TEST/fixture/file-link"
fi
if [ -p "$TMPDIR_TEST/fixture/pipe" ]; then
    run_test "FIFO classification" ./ls -F "$TMPDIR_TEST/fixture/pipe"
fi

echo
echo "=== 6. OPTION PARSING / ERROR HANDLING ==="
run_test "Options after operand" ./ls src -F
run_test "-- ends option parsing" ./ls -- src
run_error_test "Unknown option" ./ls -Z

echo
echo "=== 7. REDIRECTION / NO CRASH ==="
if ./ls -l >"$TMPDIR_TEST/redirect.txt" 2>"$TMPDIR_TEST/redirect.err"; then
    if [ -s "$TMPDIR_TEST/redirect.txt" ]; then
        echo "Test: Redirect output to file"
        printf '%sPASS%s\n\n' "$GREEN" "$NC"
        PASS=$((PASS + 1))
    else
        echo "Test: Redirect output to file"
        printf '%sFAIL%s (empty output)\n\n' "$RED" "$NC"
        FAIL=$((FAIL + 1))
    fi
else
    echo "Test: Redirect output to file"
    printf '%sFAIL%s\n\n' "$RED" "$NC"
    FAIL=$((FAIL + 1))
fi
run_test "Many separate options" ./ls -a -i -l -s -h -R -F -q .

echo
echo "=== 8. SYSTEM LS COMPARISON (INFORMATIONAL) ==="
if [ -x /bin/ls ]; then
    echo "--- System ls (first lines) ---"
    /bin/ls -l 2>&1 | sed -n '1,3p'
    echo
    echo "--- Project ls (first lines) ---"
    ./ls -l 2>&1 | sed -n '1,3p'
    echo
    echo "Note: output differences are informational, not byte-for-byte comparison."
else
    printf '%sSystem /bin/ls not found; comparison skipped.%s\n' "$YELLOW" "$NC"
    WARN=$((WARN + 1))
fi

echo
echo "======================================"
echo "TEST RESULTS"
echo "======================================"
printf 'Passed: %s%s%s\n' "$GREEN" "$PASS" "$NC"
printf 'Failed: %s%s%s\n' "$RED" "$FAIL" "$NC"
TOTAL=$((PASS + FAIL))
echo "Total: $TOTAL"
if [ "$WARN" -gt 0 ]; then
    printf 'Warnings: %s%s%s\n' "$YELLOW" "$WARN" "$NC"
fi
if [ "$FAIL" -eq 0 ]; then
    printf '%sALL TESTS PASSED%s\n' "$GREEN" "$NC"
else
    printf '%sSome tests failed. Review the output above.%s\n' "$YELLOW" "$NC"
fi
echo "======================================"

[ "$FAIL" -eq 0 ]
