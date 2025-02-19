#include <crbn_jobManager.hpp>
 
// g++ jobManagerTest.cpp -I ../../lib -I ../../lib/asio-1.30.2/include/ -lcurl -D LOGGING -std=c++20

int main()
{
    crbn::Jobs job;

    job.async_clientStart();

    // crbn::serialiser *dat = new crbn::serialiser((uint8_t *)"bingus", 7, crbn::op::ID_ENQUIRY, job.getTicket());
    // job.enqueOUT(dat);
    // dat = new crbn::serialiser((uint8_t *)"bongus", 7, crbn::op::ID_ENQUIRY, job.getTicket());
    // job.enqueOUT(dat);

    // std::vector<uint8_t> testVec;
    // testVec.reserve(500000);

    // uint8_t *raw = new uint8_t[500000]{(uint8_t)45};

    // crbn::serialiser *at2342 = new crbn::serialiser(raw, 500000, 4, 5);


    // crbn::jsn::Json_Helper j;
    // j.init();
    // j.strGet(crbn::jsn::keys::screen_height);

    // int jsonticket = job.getTicket();
    // crbn::serialiser *dat = new crbn::serialiser((uint8_t *)"1234", 4, crbn::op::ID_ENQUIRY, jsonticket);
    // job.enqueOUT(dat);

    crbn::serialiser *dat = new crbn::serialiser((uint8_t *)"1234", 4, crbn::op::IMAGE_REQUEST, job.getTicket());
    job.enqueOUT(dat);

    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // if (job.isJsonAvailable("1234"))
        {
            // std::cout << std::setw(4) << *job.getJsonData("1234") << "\n";
        }
    }
    return EXIT_SUCCESS;
}