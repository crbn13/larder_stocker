#!/bin/bash

args=''
if [[ -n "$1" ]] ; then
    args="$1"
    echo "g++ arguments provided | $1 |"
fi

(bash universalobjects.sh "-gc" "$args") 


(bash client.sh "$args") &
(bash server.sh "$args") &

wait 

echo '

    : ALL Complete :

    '