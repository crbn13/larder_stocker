g++ -I /usr/lib/asio-1.28.0/include/ learnNetworking.cpp -o client -std=c++23
cd ../../server/source
g++ -I /usr/lib/asio-1.28.0/include/ asioServer.cpp -o server -std=c++23 -I ../../lib
