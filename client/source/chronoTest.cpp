#include <chrono>
#include <crbn_logging.hpp>

int main()
{
    auto start = std::chrono::high_resolution_clock::now(); // clock used to timeout connected users which have not responded data for too long
    long int bytesRead = 0;

    while (true)
    {
        auto end = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(end - start) > std::chrono::milliseconds(1000)) // if no information has been recieved then ends connection
        {
            crbn::log("crbn::simpleNetworking::serverStart | TIMED OUT WHILE WAITING FOR BODY BYTES ");
            break;
        }
    }
    return true;

}