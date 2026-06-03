#!/bin/bash

CURR_TIME="$(date +%s)"
DIR_BACKUP="_backup-${CURR_TIME}"

mkdir -p "$DIR_BACKUP"

for f in $(find . -type f -a -not -path './.git/*'); do
  if ! $(git check-ignore -q $f);
  then

    echo "$f";

    mkdir -p "$DIR_BACKUP/$(dirname $f)"
    cp -rf "$f" "$DIR_BACKUP/$f"

  fi
done