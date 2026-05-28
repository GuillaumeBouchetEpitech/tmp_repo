#!/bin/bash


ROOT_DIR="$PWD"

DIR_THIRD_PARTIES="$ROOT_DIR/_third_parties"


mkdir -p "$DIR_THIRD_PARTIES"

#
# LUA
#

DIR_LUA_LANG="$DIR_THIRD_PARTIES/lua"

if [ ! -d "$DIR_LUA_LANG" ];
then

  git clone --depth 1 --branch v5.3.4 https://github.com/lua/lua "$DIR_LUA_LANG"

fi

if [ ! -f  "$DIR_LUA_LANG/native/liblua.a" ];
then

  cd "$DIR_LUA_LANG" || exit 1

  rm -rf *.o
  rm -rf "liblua.a"
  rm -rf "native"

  make all CC='gcc' CORE_T='liblua.a' ALL_T='liblua.a'

  mkdir -p native
  mv "liblua.a" "native/liblua.a"

  cd "$ROOT_DIR" || exit 1

fi

if [ ! -f  "$DIR_LUA_LANG/wasm/liblua.a" ];
then

  cd "$DIR_LUA_LANG" || exit 1

  rm -rf *.o
  rm -rf "liblua.a"
  rm -rf "wasm"

  make all CC='emcc' CORE_T='liblua.a' ALL_T='liblua.a'

  mkdir -p wasm
  mv "liblua.a" "wasm/liblua.a"

  cd "$ROOT_DIR" || exit 1

fi

#
# LUACPP
#

DIR_LUA_CPP="$DIR_THIRD_PARTIES/luacpp"

if [ ! -d "$DIR_LUA_CPP" ];
then

  git clone --depth 1 --branch v0.3.0 https://github.com/jordanvrtanoski/luacpp "$DIR_LUA_CPP"

fi

if [ ! -f "$DIR_LUA_CPP/build-native/libluacpp.a" ];
then

  cd "$DIR_LUA_CPP/Source" || exit 1

  rm -rf "../build-native"

  cmake -B "../build-native"
  cd "../build-native" || exit 1
  cmake --build . --target luacpp_static --config Release --parallel 5

  cd "$ROOT_DIR" || exit 1

fi

if [ ! -f "$DIR_LUA_CPP/build-wasm/libluacpp.a" ];
then

  cd "$DIR_LUA_CPP/Source" || exit 1

  rm -rf "../build-wasm"

  emcmake cmake -B "../build-wasm" -DLUA_INCLUDE_DIR="$DIR_LUA_LANG/" -DLUA_LIBRARIES="$DIR_LUA_LANG/wasm/liblua.a"
  cd "../build-wasm" || exit 1
  cmake --build . --target luacpp_static --config Release --parallel 5

  cd "$ROOT_DIR" || exit 1

fi

#
# BUILD
#

# gcc ./hello.cpp -I /usr/local/include/LuaCpp -I /usr/include/lua5.3/ -lluacpp -llua5.3 -lstdc++ -o hello
# gcc ./hello.cpp -I ./luacpp/Source -I /usr/include/lua5.3/ -L./luacpp/build/ -l:libluacpp.a -llua5.3 -lstdc++ -o hello

rm -rf ./bin
mkdir -p ./bin

gcc ./src/hello1.cpp -I "$DIR_LUA_CPP/Source/" -I "$DIR_LUA_LANG/" -L"$DIR_LUA_CPP/build-native/" -l:libluacpp.a -L"$DIR_LUA_LANG/native/" -l:liblua.a -lstdc++ -lm -o ./bin/hello1
gcc ./src/hello3.cpp -I "$DIR_LUA_CPP/Source/" -I "$DIR_LUA_LANG/" -L"$DIR_LUA_CPP/build-native/" -l:libluacpp.a -L"$DIR_LUA_LANG/native/" -l:liblua.a -lstdc++ -lm -o ./bin/hello2
gcc ./src/hello3.cpp -I "$DIR_LUA_CPP/Source/" -I "$DIR_LUA_LANG/" -L"$DIR_LUA_CPP/build-native/" -l:libluacpp.a -L"$DIR_LUA_LANG/native/" -l:liblua.a -lstdc++ -lm -o ./bin/hello3

#
#
#

rm -rf ./dist
mkdir -p ./dist

em++ ./src/hello1.cpp -I "$DIR_LUA_CPP/Source/" -I "$DIR_LUA_LANG/" -L"$DIR_LUA_CPP/build-wasm/" -l:libluacpp.a -L"$DIR_LUA_LANG/wasm/" -l:liblua.a -lstdc++ -lm -o ./dist/hello1.html
em++ ./src/hello2.cpp -I "$DIR_LUA_CPP/Source/" -I "$DIR_LUA_LANG/" -L"$DIR_LUA_CPP/build-wasm/" -l:libluacpp.a -L"$DIR_LUA_LANG/wasm/" -l:liblua.a -lstdc++ -lm -o ./dist/hello2.html
em++ ./src/hello3.cpp -I "$DIR_LUA_CPP/Source/" -I "$DIR_LUA_LANG/" -L"$DIR_LUA_CPP/build-wasm/" -l:libluacpp.a -L"$DIR_LUA_LANG/wasm/" -l:liblua.a -lstdc++ -lm -o ./dist/hello3.html --preload-file ./src/hello3.lua


