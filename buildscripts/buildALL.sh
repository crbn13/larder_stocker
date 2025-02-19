#!/bin/bash

args=''
if [[ -n "$1" ]] ; then
    args="$1"
    echo "g++ arguments provided | $1 |"
fi

(bash precompileheaders.sh   y "$args") &
(bash universalobjects.sh y "$args") &

wait

(bash client.sh "$args") &
(bash server.sh "$args") &

wait 

echo '

    : ALL Complete :

    '