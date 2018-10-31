#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Please specify path.";
    exit -1;
fi

for var123 in "$1"*
do
  if [[ -f $var123 && -x $var123 ]]; then 
    echo "$var123**"
  elif [[ -d $var123 ]]; then 
    echo "$var123/"
  else
    echo "$var123"
 fi
done