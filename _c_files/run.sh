#!/usr/bin/env bash
set -e

gcc chash.c hash.c rwlock.c -o chash -pthread
./chash

echo
echo "===== hash.log ====="
cat hash.log