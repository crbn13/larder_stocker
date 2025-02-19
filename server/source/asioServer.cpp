
#include <iostream>
#include <chrono>
#include <utility>
#include <ctime>
// #define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

//#include "../../lib/nlohmann/json.hpp"

#include "../../lib/dataTransfer.hpp"

std::vector<char> vBuffer(1 * 1084);

std::vector<uint8_t> v_headerbuffer(1 * 10);

void grabSomeData(asio::ip::tcp::socket &socket)
{
  socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()), // lamda function im scraedddddd

                         [&](std::error_code ec, std::size_t length)
                         {
                           if (!ec)
                           {
                             std::cout << "\n\nRead " << length << " bytes\n\n";

                             for (int i = 0; i < length; i++)
                               std::cout << vBuffer[i];

                             grabSomeData(socket);
                           }
                         });
}

int main()
{

  try
  {
    std::cout << " hello there world :) \n";

    asio::error_code ec;

    // create a "context " essentially the platform specific interface
    asio::io_context context;

    for (;;)
    {
      std::cout << " waiting for client connection \n";

      asio::ip::tcp::acceptor acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 5476));
      asio::ip::tcp::socket socket(context);
      acceptor.accept(socket);
      // grabSomeData(socket);

      if (acceptor.is_open())
      {
        std::cout << " client connected maby ? :p \n";
      }
      else
      {
        std::cout << ec << std::endl;
      }

      size_t bytes = socket.available();

      const auto tStart = clock();


      std::cout << " time check " << tStart << " | " << clock() << std::endl;
      
      while (bytes < 10)
      {
        std::cout << " waiting for data \n "
                  << " there are currently "
                  << bytes
                  << " available bytes to read \n";
        bytes = socket.available();
        if ((clock() - tStart) > 10000000)
        {
          std::cout << " client response timed out \n";
          break;
        }
      }

      std::cout << std::endl
                << std::endl;

      if (bytes >= 10)
      {
        std::cout << " enough data input to read header \n";

        socket.read_some(asio::buffer(v_headerbuffer.data(), v_headerbuffer.size()), ec);

        uint8_t *arr;
        arr = new uint8_t[v_headerbuffer.size()];
        for (int i = 0; i < v_headerbuffer.size(); i++)
        {
          arr[i] = v_headerbuffer[i];
        }
        networkingData data(arr);
        std::cout << "\n the size of the body is " << data.header.bodysize << std::endl;

        uint8_t *arr2;
        arr2 = new uint8_t[data.header.bodysize];

        bytes = socket.available();

        std::cout << " there are " << bytes
                  << " available bytes to read \n";

        while (bytes < data.header.bodysize)
        {
          std::cout << " waiting for data \n "
                    << " there are currently "
                    << bytes
                    << " available bytes to read \n";
          bytes = socket.available();
        }

        if (bytes > data.header.bodysize || bytes == data.header.bodysize)
        {
          std::cout << " reading body \n";
          socket.read_some(asio::buffer(arr2, data.header.bodysize), ec);
          data.bodyInput(arr2);

          for (int i = 0; i < data.header.bodysize; i++)
          {
            std::cout << (char)+data.body.rawData[i] << " ";
          }
          std::cout << std::endl;

          //std::string inp;
          // std::cin >> inp;
        }
      }
    }
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
}
