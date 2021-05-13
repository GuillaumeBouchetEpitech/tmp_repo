
#include "multithreading/Producer.hpp"

#include "utilities/TraceLogger.hpp"

#include <cstdlib> // EXIT_SUCCESS
#include <iostream>
#include <mutex>

int main()
{
    D_MYLOG("start");

    {
        std::mutex loggerMutex;

        multithreading::Producer producer;

        producer.initialise(3);

        for (int ii = 0; ii < 20; ++ii)
        {
            producer.push([ii, &loggerMutex]()
            {
                // thread safe logging
                std::unique_lock<std::mutex> lock(loggerMutex);

                D_MYLOG("task " << ii);
            });
        }

        {
            // thread safe logging
            std::unique_lock<std::mutex> lock(loggerMutex);

            D_MYLOG("main waiting");
        }

        producer.waitUntilAllCompleted();

        D_MYLOG("main done");
    }

    D_MYLOG("stop");

    return EXIT_SUCCESS;
}
