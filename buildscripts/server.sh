#!/bin/bash

args=''
if [[ -n "$1" ]] ; then
    args="$1"
    echo "g++ arguments provided | $1 |"
fi

rm ../builds/server
g++ ../objects/server/* $args -o ../builds/server -flto -lcurl 
if [[ $? -eq 0 ]] ; then
    echo " Successfully compiled server "
fi