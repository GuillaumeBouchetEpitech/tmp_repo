#!/bin/bash


#
# LUA (CLONE)
#

CURR_DIR=$PWD

LUA_DIR=./third_parties/lua-v5.3.4
LUACPP_DIR=./third_parties/luacpp-v0.3.0

mkdir -p ./third_parties

if [ ! -d "$LUA_DIR" ];
then

  git clone --depth 1 --branch v5.3.4 https://github.com/lua/lua "$LUA_DIR"

fi

#
# LUA (COMPILE)
#

if [ ! -f "$LUA_DIR/liblua.a" ];
then

  cd "$LUA_DIR" || exit 1

  # force "gcc" instead of "clang"
  make liblua.a CC='gcc'

  cd "$CURR_DIR" || exit 1

fi

#
# LUACPP (CLONE)
#

if [ ! -d "$LUACPP_DIR" ];
then

  git clone --depth 1 --branch v0.3.0 https://github.com/jordanvrtanoski/luacpp "$LUACPP_DIR"

fi

#
# LUACPP (COMPILE)
#

if [ ! -f "$LUACPP_DIR/build/libluacpp.a" ];
then

  cd "$LUACPP_DIR" || exit 1

  mkdir -p ./build
  cd ./build || exit 1
  cmake ../Source
  make luacpp_static -j 6

  cd "$CURR_DIR" || exit 1

fi

#
# BUILD
#

mkdir -p ./bin

gcc ./src/hello1.cpp -I "$LUACPP_DIR/Source" -I "$LUA_DIR/" -L"$LUACPP_DIR/build/" -l:libluacpp.a -L"$LUA_DIR/" -l:liblua.a -lstdc++ -lm -o ./bin/hello1
gcc ./src/hello2.cpp -I "$LUACPP_DIR/Source" -I "$LUA_DIR/" -L"$LUACPP_DIR/build/" -l:libluacpp.a -L"$LUA_DIR/" -l:liblua.a -lstdc++ -lm -o ./bin/hello2


