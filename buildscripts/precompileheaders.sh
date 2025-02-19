#!/bin/bash



echo '
        |$
Usage : |$ bash precompileheaders.sh "y/n remove files without prompt" "optional compiler args" 
        |$
'

echo " Removing prexisting object files " 



declare rmargs="-i"   
if [[ "$1" == "y" ]] ; then
    rmargs=''
fi


args=''
if [[ -n "$2" ]] ; then
    args="$2"
    echo "g++ arguments provided | $2 |"
fi

in_olc="../lib/olcPixelGameEngine.cpp"
out_olc="../objects/client/olcPixelGameEngine.o"

in_asio="../lib/asio.cpp"
out_asio="../objects/client/asio.o"
cp_asio="../objects/server/asio.o"

in_json="../lib/nlohmannjson.cpp"
out_json="../lib/nlohmann/json.hpp.gch"
# out_json="../objects/client/json.o"
# cp_json="../objects/server/json.o"

in_stb="../lib/stb_image.cpp"
out_stb="../objects/server/stb_image.o"

echo " Removing old files "

#array of variables with names beginning with out
declare -a arr=${!out*}
# echo $arr
# delete all prexisting files

for i in ${arr[@]}
do
    # echo "i = $i"
    #delete optional files if they exist
    string="cp_"${i:4}
    # echo ${!string} ## outputs the value of the variable with its name stored in string
    
    file=${!string}
    if [ ! -v ${!string} ]; then 
        # echo "${!string} is set";
        if [[ -a ${file} ]] ; then
            rm $rmargs -v "${!string}"
        else 
            echo "${!string} not found"
        fi
    fi
    #delete normal files
    if [[ -a ${!i} ]] ; then
        rm $rmargs -v "${!i}"
    else
        echo "${!i} not found "
    fi
done

echo "";

(
    if [[ -a ${out_stb} ]] ; then 
        echo "Already Compiled $in_stb to $out_stb"
    else 
        g++ -c $in_stb                                  \
        -o  $out_stb                                    \
        -O3 -Ofast -Os -s $args                       \
        -march=native                                   \
        -std=c++17
        if [ $? -eq 0 ] ; then echo -e " Compiled  | $in_stb |\n\tTo | $out_stb |" ; fi
    fi
) &



(    
    if [[ -a "$out_olc" ]] ; then 
        echo "Already Compiled $in_olc  to $out_olc"
    else 
        g++ -c $in_olc                                      \
            -o  $out_olc                                    \
            -lX11 -lGL -lpthread -lpng -lstdc++fs           \
            -std=c++17           $args                      \
            -O3 -Ofast -Os -s                               \
            -march=native                                   \
            -std=c++17           
            #-H -v                          
        if [ $? -eq 0 ] ; then echo -e " Compiled  | $in_olc |\n\tTo | $out_olc |" ; fi
    fi
) &



(
    if [[ -a "$out_asio" ]] && [[ -a "$cp_asio" ]] ; then 
        echo "Already Compiled $in_asio to $out_asio and $cp_asio"
    else 
        g++ -c $in_asio                                     \
            -o  $out_asio                                   \
            -O3 -Ofast -Os -s            $args              \
            -I ../lib/asio-1.30.2/include/                  \
            -march=native                                   \
            -std=c++17                                                         
        if [ $? -eq 0 ] ; then 
            cp "$out_asio" "$cp_asio"
            echo -e " Compiled  | $in_asio |\n\tTo | $out_asio and $cp_asio | " 
        fi
    fi
) &



#(
#    if [[ -a "$out_json" ]] && [[ -a "$cp_json" ]] ; then 
#        echo "Already Compiled $in_json to $out_json"
#    else 
#        g++ -c $in_json                                     \
#            -o  $out_json                                   \
#            -O3 -Ofast -Os -s                               \
#            -I ../lib/                                      \
#            -march=native                                   \
#            -std=c++17                                                         
#        if [ $? -eq 0 ] ; then 
#            cp "$out_json" "$cp_json"
#            echo -e " Compiled  | $in_json |\n\tTo | $out_json and $cp_json | "
#        fi
#    fi
#) &

(
    if [[ -a "$out_json" ]] ; then 
        echo "Already Compiled $in_json to $out_json"
    else 
        g++ -c $in_json                                     \
            -o  $out_json                                   \
            -O3 -Ofast -Os -s                               \
            -I ../lib/                                      \
            -march=native        $args                      \
            -std=c++17                                                         
        if [ $? -eq 0 ] ; then 
            echo -e " Compiled  | $in_json |\n\tTo | $out_json |"
        fi
    fi
) &

wait

echo ' 
    : Complete :
'