#!/bin/bash
if [[ $# == 2 ]]
then
  if [[ -f "$1" && -x "$2" ]]
  then
    if gcc "$1" -o "$2"
    then
      ./"$2"
    else
      echo "smth went wrong :c"
    fi
  else
    echo "u have 2 enter valid parameters" >&2
  fi
else
  echo "ur amount of parameters must b 2"
fi
