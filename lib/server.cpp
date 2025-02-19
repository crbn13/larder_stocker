#include "headers/crbn_jobManager.hpp"

int main()
{
    crbn::Jobs server;
    server.async_serverStart();

    for (;;)
    {
    }
    return EXIT_SUCCESS;
}