#!/bin/bash

clear

BASE_DIR=$PWD

echo ""
echo "#"
echo "# BUILDING"
echo "#"
echo ""

rm -rf ./bin || true
rm -rf ./_cmake.build || true
cmake -B ./_cmake.build || exit 1
cd ./_cmake.build || exit 1
cmake --build . --config Release --parallel 5 || exit 1
cd "$BASE_DIR" || exit 1

echo ""
echo "#"
echo "# RUNNING"
echo "#"
echo ""

./bin/exec

