#!/bin/bash


INITIAL_CWD="$PWD"

#
#

echo ""
echo "#"
echo "# NATIVE BUILD"
echo "#"
echo ""

cd "$INITIAL_CWD/tests" || exit 1

cmake -B "./_cmake-build.release.native" -DCMAKE_C_COMPILER_LAUNCHER=/usr/bin/sccache -DCMAKE_CXX_COMPILER_LAUNCHER=/usr/bin/sccache
cd "./_cmake-build.release.native" || exit 1
mold -run cmake --build . --config Release --parallel 5

#
#

# echo ""
# echo "#"
# echo "# PRINT"
# echo "#"
# echo ""

# cd "$INITIAL_CWD" || exit 1

# tree -hD ./lib/wasm
# tree -hD ./lib/native
# # tree -hD ./samples/test-bed/bin
# # tree -hD ./samples/test-bed/dist

echo ""
echo "#"
echo "# DONE!"
echo "#"
echo ""

