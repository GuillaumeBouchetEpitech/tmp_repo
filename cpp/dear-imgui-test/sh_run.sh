#!/bin/bash

if [ ! -d ./third_parties/imgui ];
then

  mkdir -p ./third_parties

  git clone \
    --depth 1 \
    --branch v1.92.8-docking \
    https://github.com/ocornut/imgui.git \
    ./third_parties/imgui

fi


ROOT_DIR=$PWD

cd ./third_parties/imgui/examples/example_sdl2_opengl3 || exit 1

make -f Makefile clean
make -f Makefile -j 4

mkdir -p "$ROOT_DIR/bin"
cp ./example_sdl2_opengl3 "$ROOT_DIR/bin/example_sdl2_opengl3"

make -f Makefile.emscripten clean
make -f Makefile.emscripten -j 4

cp -r ./web "$ROOT_DIR/wasm-build"
