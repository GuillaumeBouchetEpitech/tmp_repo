#!/bin/bash

# echo "$0"

CURR_TIME="$(date +%s)"
DIR_BACKUP="_backup-${CURR_TIME}"

mkdir -p "$DIR_BACKUP"

# shellcheck disable=SC2329
my_func () {
  DIR_BACKUP="$1"
  CURR_FILE="$2"

  echo " -> $CURR_FILE"

  if ! $(git check-ignore -q "$CURR_FILE");
  then
    mkdir -p "$DIR_BACKUP/$(dirname "$CURR_FILE")"
    cp -rf "$CURR_FILE" "$DIR_BACKUP/$CURR_FILE"
  fi
}
export -f my_func

find . -type f \
  -and -not -path './.git/*' \
  -and -not -path './_backup-*/*' \
  -and -not -path '*/_cmake-*/*' \
  -and -not -path '*/_bin/*' \
  -exec bash -c 'my_func $0 $1' "$DIR_BACKUP" "{}" \;

exit 0

# for f in $(find . -type f -a -not -path './.git/*'); do
#   if ! $(git check-ignore -q $f);
#   then

#     echo "$f";

#     mkdir -p "$DIR_BACKUP/$(dirname "$f")"
#     cp -rf "$f" "$DIR_BACKUP/$f"

#   fi
# done