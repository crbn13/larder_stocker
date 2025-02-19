g++ client.cpp -o gui -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++20 -I . -I ../../lib -I ../../lib/asio-1.30.2/include -D LOGGING -D DEF_CLIENT #-O3 -Os -s -Lfast -ftree-vectorize -Wall -Werror -pedantic
#./gui 250 250 1
