#!/bin/bash

args='' 
if [[ -n $1 ]] ; then
    args="$1"
    echo "g++ arguments provided | $1 |"
fi

rm ../builds/client
g++ ../objects/client/* $args -o ../builds/client -flto -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++20  

if [[ $? -eq 0 ]] ; then
    echo " Successfully compiled client "
fi
    