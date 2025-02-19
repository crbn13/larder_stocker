#!/bin/bash

echo '
        |$
Usage : |$ bash precompileheaders.sh "y/n remove files without prompt" "optional compiler args" 
        |$
'

echo " Removing prexisting object files " 

if [[ "$1" == "y" ]] ; then
    rm -v ../objects/*/crbn*.o ../objects/crbn*.o 
else
    rm -i -v ../objects/*/crbn*.o ../objects/crbn*.o 
fi

args=''
setarg=" -D ASIO_SEPARATE_COMPILATION -Wall "
if [[ -n "$2" ]] ; then
    args=$setarg$2
    echo "g++ arguments provided | $args |"
fi

echo '
    : Compiling :
'

in_ds="../lib/crbn_dataSerializer.cpp"
out_ds="../objects/client/crbn_dataSerializer.o"
cp_ds="../objects/server/crbn_dataSerializer.o"
in_ge="../lib/crbn_graphEstimation.cpp"
out_ge="../objects/server/crbn_graphEstimation.o"
in_gui="../lib/crbn_gui.cpp"
out_gui="../objects/client/crbn_gui.o"
in_imd="../lib/crbn_imageDownloader.cpp"
out_imd="../objects/server/crbn_imageDownloader.o"
in_jsn="../lib/crbn_json.cpp"
out_jsn="../objects/client/crbn_json.o"
cp_jsn="../objects/server/crbn_json.o"
in_jobmc="../lib/crbn_jobManager.cpp"
out_jobmc="../objects/client/crbn_jobManager.o"
in_jobms="../lib/crbn_jobManager.cpp"
out_jobms="../objects/server/crbn_jobManager.o"
in_log="../lib/crbn_logging.cpp"
out_log="../objects/client/crbn_logging.o"
cp_log="../objects/server/crbn_logging.o"
in_pgesm="../lib/crbn_PGE_SceneManager.cpp"
out_pgesm="../objects/client/crbn_PGE_SceneManager.o"
in_scr="../lib/crbn_screens.cpp"
out_scr="../objects/client/crbn_screens.o"
in_simnet="../lib/crbn_simpleNetworking.cpp"
out_simnet="../objects/client/crbn_simpleNetworking.o"
cp_simnet="../objects/server/crbn_simpleNetworking.o"
in_apik="../lib/apikeys.cpp"
out_apik="../objects/server/crbn_apikeys.o"
in_svr="../lib/server.cpp "
out_svr="../objects/server/crbn_server.o"
in_cli="../client/source/client.cpp"
out_cli="../objects/client/crbn_client.o"

#~~~~~~~~~~~~~~~~~~~~~~~#
#(
#    if [[ -a "$out_replace" ]] ; then 
#        echo "Already Compiled $in_replace to $out_replace"
#    else 
#
#        if [ $? -eq 0 ] ; then echo " Compiled | $in_replace | to $out_replace" ; fi
#    fi
#) &
#~~~~~~~~~~~~~~~~~~~~~~~#

(
    if [[ -a "$out_ds" ]] && [[ -a "$cp_ds" ]] ; then 
        echo "Already Compiled $in_ds to $out_ds and $cp_ds"
    else 
        g++ -c $in_ds \
            -o $out_ds \
            $args \
            -O3 -Ofast -Os -s          \
            -march=native\
            -std=c++17 
        if [ $? -eq 0 ] ; then 
            echo " Compiled | $in_ds | to $out_ds" 
            cp "$out_ds" "$cp_ds"
        fi
    fi
) &
    #-Wall -Wextra -pedantic -Werror

(
    if [[ -a "$out_ge" ]] ; then 
        echo "Already Compiled $in_ge to $out_ge"
    else 
        g++ -c $in_ge \
            -o  $out_ge \
            $args \
            -O3 -Ofast -Os -s          \
            -march=native\
            -std=c++17
        if [ $? -eq 0 ] ; then echo " Compiled | $in_ge | to $out_ge" ; fi
    fi
) &
    #-Wall -Wextra -pedantic -Werror

(
    if [[ -a "$out_gui" ]] ; then 
        echo "Already Compiled $in_gui to $out_gui"
    else 
        g++ -c $in_gui \
            -o  $out_gui \
            $args \
            -O3 -Ofast -Os -s           \
            -I ../lib/asio-1.30.2/include/ -I ../lib/ \
            -march=native \
            -D DEF_CLIENT \
            -std=c++17
        if [ $? -eq 0 ] ; then echo " Compiled | $in_gui | to $out_gui" ; fi
    fi
) &

(
    if [[ -a "$out_imd" ]] ; then 
        echo "Already Compiled $in_imd to $out_imd"
    else 
        g++ -c $in_imd       \
            -o  $out_imd \
            $args \
            -O3 -Ofast -Os -s           \
            -I ../lib/asio-1.30.2/include/ -I ../lib/ \
            -lcurl \
            -march=native \
            -std=c++17
        if [ $? -eq 0 ] ; then echo " Compiled | $in_imd | to $out_imd" ; fi
    fi
) &

(
    if [[ -a "$out_jsn" ]] && [[ -a "$cp_jsn" ]]; then 
        echo "Already Compiled $in_jsn to $out_jsn and $cp_jsn"
    else 
        g++ -c $in_jsn   \
            -o  $out_jsn  \
            $args \
            -O3 -Ofast -Os -s          \
            -march=native \
            -std=c++17
        if [ $? -eq 0 ] ; then 
            echo " Compiled | $in_jsn | to $out_jsn" 
            cp "$out_jsn" "$cp_jsn"
        fi
    fi
) &

(
    if [[ -a "$out_jobmc" ]] ; then 
        echo "Already Compiled $in_jobmc to $out_jobmc"
    else 
        g++ -c $in_jobmc       \
            -o  $out_jobmc \
            $args \
            -I ../lib/asio-1.30.2/include/ \
            -O3 -Ofast -Os -s           \
            -march=native \
            -std=c++20 \
            -D DEF_CLIENT
        if [ $? -eq 0 ] ; then echo " Compiled | $in_jobmc | to $out_jobmc" ; fi
    fi
) &

(
    if [[ -a "$out_jobms" ]] ; then 
        echo "Already Compiled $in_jobms to $out_jobms"
    else 
        g++ -c $in_jobms        \
            -o  $out_jobms  \
            $args \
            -I ../lib/asio-1.30.2/include/ \
            -O3 -Ofast -Os -s           \
            -march=native \
            -std=c++17 \
            -D DEF_SERVER
        if [ $? -eq 0 ] ; then echo " Compiled | $in_jobms | to $out_jobms" ; fi
    fi
) &

(
    if [[ -a "$out_log" ]] && [[ -a "$cp_log" ]] ; then 
        echo "Already Compiled $in_log to $out_log and $cp_log"
    else 
        g++ -c  $in_log        \
            -o   $out_log\
            $args \
            -O3 -Ofast -Os -s           \
            -march=native \
            -std=c++17
        if [ $? -eq 0 ] ; then 
            echo " Compiled | $in_log | to $out_log" 
            cp "$out_log" "$cp_log"
        fi
    fi
) &

(
    if [[ -a "$out_pgesm" ]] ; then 
        echo "Already Compiled $in_pgesm to $out_pgesm"
    else 
        g++ -c $in_pgesm      \
            -o  $out_pgesm \
            $args \
            -O3 -Ofast -Os -s                               \
            -march=native                                   \
            -std=c++17
        if [ $? -eq 0 ] ; then echo " Compiled | $in_pgesm | to $out_pgesm" ; fi
    fi
) &

(
    if [[ -a "$out_scr" ]] ; then 
        echo "Already Compiled $in_scr to $out_scr "
    else 
        g++ -c $in_scr              \
            -o  $out_scr \
            $args \
            -O3 -Ofast -Os -s                      \
            -march=native                          \
            -std=c++17                             \
            -I ../lib/asio-1.30.2/include/         \
            -D DEF_CLIENT
        if [ $? -eq 0 ] ; then echo " Compiled | $in_scr | to $out_scr" ; fi
    fi
) &                            

(
    if [[ -a "$out_simnet" ]] && [[ -a "$cp_simnet" ]] ; then 
        echo "Already Compiled $in_simnet to $out_simnet and $cp_simnet "
    else 
        g++ -c $in_simnet                \
            -o  $out_simnet \
            $args \
            -O3 -Ofast -Os -s                               \
            -march=native                                   \
            -std=c++17                                      \
            -I ../lib/asio-1.30.2/include/
        if [ $? -eq 0 ] ; then 
            echo " Compiled | $in_simnet | to $out_simnet"
            cp "$out_simnet" "$cp_simnet"
        fi
    fi
) &

(
    if [[ -a "$out_apik" ]] ; then 
        echo "Already Compiled $in_apik to $out_apik"
    else 
        g++ -c $in_apik              \
            -o  $out_apik \
            $args \
            -O3 -Ofast -Os -s                               \
            -march=native                                   \
            -std=c++17                                      \
            -I ../lib/asio-1.30.2/include/
        if [ $? -eq 0 ] ; then echo " Compiled | $in_apik | to $out_apik" ; fi
    fi
) &

(
    if [[ -a $out_cli ]] ; then 
        echo "Already Compiled $in_cli to $out_cli"
    else 
        g++ -c $in_cli                \
            -o  $out_cli  \
            $args \
            -O3 -Ofast -Os -s                               \
            -march=native                                   \
            -std=c++20                                      \
            -D DEF_CLIENT \
            -I ../lib/asio-1.30.2/include
        if [ $? -eq 0 ] ; then echo " Compiled | $in_cli | to $out_cli" ; fi
    fi
) &

(
    if [[ -a "$out_svr" ]] ; then 
        echo "Already Compiled $in_svr to $out_svr"
    else 
        g++ -c $in_svr              \
            -o  $out_svr \
            $args \
            -O3 -Ofast -Os -s                               \
            -march=native                                   \
            -std=c++17                                      \
            -D DEF_SERVER \
            -I ../lib/asio-1.30.2/include
        if [ $? -eq 0 ] ; then echo " Compiled | $in_svr | to $out_svr" ; fi
    fi
) &

wait
 
echo '
    : Compiling Finished :  
'
