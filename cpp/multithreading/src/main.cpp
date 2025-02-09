
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

    { // test

        multithreading::Producer producer;
        producer.initialise(3);

        constexpr int totalRepeat = 200;
        constexpr int totalTasks = 27 * 27 * 27;

        {
            auto start = std::chrono::steady_clock::now();
            {
                for (volatile int ii = 0; ii < totalRepeat; ++ii)
                {
                    for (volatile int jj = 0; jj < totalTasks; ++jj)
                        ;
                    // producer.push([ii]()
                    // {
                    // });
                }

                producer.waitUntilAllCompleted();
            }
            auto stop = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            D_MYLOG("duration=" << duration << "ms");
        }

        {
            auto start = std::chrono::steady_clock::now();
            {
                for (volatile int ii = 0; ii < totalRepeat; ++ii)
                {
                    producer.push([]()
                    {
                        for (volatile int jj = 0; jj < totalTasks; ++jj)
                            ;
                    });
                }

                producer.waitUntilAllCompleted();
            }
            auto stop = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            D_MYLOG("duration=" << duration << "ms");
        }

    } // test

    D_MYLOG("stop");

    return EXIT_SUCCESS;
}
