#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Please specify path.";
    exit -1;
fi


for var123 in "$1"*
do
  if [[ -f $var123 && -x $var123 ]]; then 
    rm -f $var123
  elif [[ -d $var123 ]]; then 
    rm -r $var123
  else
    rm -f $var123
 fi
done