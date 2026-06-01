#!/bin/bash

clear

echo ""
echo "############"
echo "# BUILDING #"
echo "############"
echo ""

rm -rf \
  ./tests/_cmake-build.release.native \
  ./tests/_bin

sh ./scripts/sh_build_with_cmake.sh || exit 1

echo ""
echo "###########"
echo "# TESTING #"
echo "###########"
echo ""

if [ -f ./tests/_bin/custom-container-unit-tests ];
then
  valgrind ./tests/_bin/custom-container-unit-tests
fi
