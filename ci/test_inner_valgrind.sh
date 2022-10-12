#!/usr/bin/env bash
# -x Exit immediately if any command fails
# -e Echo all commands being executed.
# -u fail for undefined variables
set -xeu
echo "Run tests in docker"
php --version
php --ini
export REPORT_EXIT_STATUS=1
if ! make test TESTS="-P -q --show-diff -m"; then
    echo "Tests failed"
    head -n999 tests/*.mem
    exit 1
fi

echo "Test that package.xml is valid"
pecl package
