#!/bin/bash
g++ ../client/source/client.cpp  -o ../builds/client \
   -I ../lib/asio-1.30.2/include/ \
   -lX11 -lGL -lpthread -lpng -lstdc++fs \
   -D LOGGING \
   -D DEF_CLIENT \
   ../lib/crbn_dataSerializer.cpp \
   ../lib/crbn_errorCodes.cpp \
   ../lib/crbn_gui.cpp \
   ../lib/crbn_jobManager.cpp \
   ../lib/crbn_json.cpp \
   ../lib/crbn_screens.cpp \
   ../lib/crbn_simpleNetworking.cpp \
   -std=c++20

