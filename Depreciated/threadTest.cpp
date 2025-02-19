#include <thread>
#include <iostream>
#include <chrono>
#include <atomic>

void sleepTimer(std::atomic<bool> *thrdone)
{
    uint16_t num = -1;
    uint64_t num2 = 1;

    for (int i = 0; i < +num; i++)
    {
        num2++;
        //std::cout << num2 << std::endl;
    }
    *thrdone = true;
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main()
{
    std::atomic<bool> threadDone = false;

    std::thread testThr(sleepTimer, &threadDone);

    while (!threadDone)
    {
        std::cout << " thread not done yet :sadface: \n"; 
    }

    testThr.join();
    std::cout << " thread Joined ! \n";

    return 0;
}