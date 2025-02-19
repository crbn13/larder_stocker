#include <utility>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>

#include <simpleNetworking.hpp>

int main()
{

    simpleNetworking net;
    std::thread thr(&simpleNetworking::serverStart, &net);

    for (;;)
    {

        if (thr.joinable())
            thr.join();
        main();
    }

    return 0;
}