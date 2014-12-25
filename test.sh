#!/usr/bin/env bash

# test script for sqr

# set -o errexit # testing, failures are good.
set -o pipefail
set -o nounset
# set -o xtrace

usage() {
	echo "${0} command logfile"
}

# config
if test "${#}" -eq 2; then
	CMD="${1}"
	LOG_FILE="${2}"
else
	usage;
	exit 1
fi

# totals
PASS=0
FAIL=0

# what to do on failure
fail() {
	((FAIL++))
	echo "fail!"
}

# what to do on success
pass() {
	((PASS++))
	echo "pass!"
}

summary() {
	echo "passes: ${PASS}"
	echo "failures: ${FAIL}"

	# exit unhappily on failures.
	if test "${FAIL}" -gt 0; then
		exit 1
	fi
}

# do_tests runs all tests
# as a function, io can be redirected as needed.
do_tests() {
	# tests go here.
	echo "no tests as of now."
}

do_tests > "${LOG_FILE}"

summary # summarize testing
