#!/bin/bash


ABS_FILE_PATH="$(realpath "${0}")"
ABS_FOLDER_PATH="$(dirname "${ABS_FILE_PATH}")"


# echo "ABS_FILE_PATH ${ABS_FILE_PATH}"
# echo "ABS_FOLDER_PATH ${ABS_FOLDER_PATH}"

pushd "$ABS_FOLDER_PATH" || exit 1



if [ ! -d ./third_parties/cglm ]; then
  mkdir -p ./third_parties
  pushd ./third_parties || exit 1
  git clone --depth 1 --branch v0.9.6 https://github.com/recp/cglm.git || exit 1
  popd || exit 1
fi


rm -rf ./out ./bin ./dist
mkdir -p ./out ./bin ./dist

echo ""
echo "#"
echo "# TRANSPILE"
echo "#"
echo ""

# "$ZC_ROOT/zc" transpile -I./third_parties/cglm/include/ ./src/main.zc -o ./out/transpiled.c || exit 1
# zc transpile -g3 -I./third_parties/cglm/include/ ./src/main.zc -o ./out/transpiled.c || exit 1
/home/barbie/Documents/_PROGRAMMING/tests/zen-c/master/zenc/zc transpile -g3 -I./third_parties/cglm/include/ ./src/main.zc -o ./out/transpiled.c || exit 1

echo ""
echo "#"
echo "# COMPILE (native)"
echo "#"
echo ""

cc \
  -O0 \
  $(pkg-config --cflags glfw3 sdl2 gl) \
  -I./third_parties/cglm/include/ \
  ./out/transpiled.c \
  $(pkg-config --static --libs glfw3 sdl2 gl) \
  -o ./bin/exec \
  || exit 1

echo ""
echo "#"
echo "# COMPILE (wasm)"
echo "#"
echo ""

# SUPPRESSED_WARNINGS="-Wno-parentheses-equality -Wno-unused-value"
# SUPPRESSED_WARNINGS="-Wno-warning"
SUPPRESSED_WARNINGS="-w" # /!\ no more warning /!\
emcc $SUPPRESSED_WARNINGS \
  -O0 \
  -I./third_parties/cglm/include/ \
  ./out/transpiled.c \
  -o ./dist/index.html \
  -s USE_SDL=2 \
  -s USE_WEBGL2=1 \
  -s FULL_ES3=1 \
  -s WASM=1 \
  -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
  || exit 1

  # -s USE_GLFW=3 \


popd || exit 1


