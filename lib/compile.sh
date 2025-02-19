g++ server.cpp -pthread -I . -I asio-1.30.2/include/ -D LOGGING -o server -lcurl -std=c++20 -I lib/ -D DEF_SERVER

#g++ jobTest.cpp -pthread -I . -I asio-1.28.0/include/ -D LOGGING -o client
