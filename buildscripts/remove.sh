#!/bin/bash
echo "This program adds the crbn_ and .o for you, also goes to both paths"
read -a array <<<"$1"
echo "${array[0]} ${array[1]}"

for i in ${array[@]}; do
  cd ../objects/server
  rm -v crbn_$i.o
  cd ../client/
  rm -v crbn_$i.o
  cd ../../buildscripts
done
