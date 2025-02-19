#include <iostream>
#include <chrono>
#include <utility>
#include <vector>
// #define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include "../../lib/dataTransfer.hpp"

std::vector<char> vBuffer(1 * 1084);

uint8_t *data(size_t *size, uint8_t *arr)
{
  uint16_t operation = 3;

  std::cout << " body size = " << *size
            << " | total size = " << *size + 10 << std::endl;

  networkingData dat(arr, operation, *size);

  uint8_t *rawArray = new uint8_t[dat.bodySize() + HEADER_SIZE_T];
  rawArray = dat.rawDatOut();

  std::cout << size << std::endl;
  for (int i = 0; i < (long unsigned int)*size + HEADER_SIZE_T; i++)
    std::cout << +rawArray[i] << " ";
  std::cout << std::endl;

  *size += HEADER_SIZE_T; // edits size so that it represents the total size of the data
  return rawArray;
}

int main(int argc, char *argv[])
{
  std::cout << " hello there world :) \n";

  asio::error_code ec;

  // create a "context " essentially the platform specific interface
  asio::io_context context;

  asio::io_context::work idleWork(context);

  // get the address of to somewher3e we wish to connect to
  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("192.168.1.110", ec), 5476);

  asio::ip::tcp::socket socket(context);

  socket.connect(endpoint, ec);

  if (!ec)
  {
    std::cout << " connected ! " << std::endl;
  }
  else
  {
    std::cout << " Failed to connect to address : \n " << ec.message() << std::endl;
  }

  // convert array into vector

  if (socket.is_open())
  {

    std::cout << " trying to write some data \n";

    std::cout << argv[1] << std::endl;

    char charchar = '1';
    int length = 0;

    while (charchar != char('\0'))
    {
      charchar = argv[1][length];
      length++;
    }
    std::cout << std::endl;

    std::cout << " length = " << length
              << " sizeof() = " << sizeof(argv[1]) << std::endl;

    size_t size = size_t(length);

    uint8_t *arr = data(&size, (uint8_t *)argv[1]);

    socket.write_some(asio::buffer(arr, size), ec);

    uint8_t *arr2[10];

    // reads comfirmation message that server has recieved data,
    // if the requrest is not recieved then the client is to assume that the server has not recieved request

    /*
    while (socket.available() < 10)
    {
      std::cout << " whomp hwomp \n";
    }
    */

    if (socket.available() == 10 || socket.available() > 10)
    {
      socket.read_some(asio::buffer(arr2, 10), ec);
    }
  }
  else
  {
    std::cout << " socket not open \n";
  }

  return 0;
}
