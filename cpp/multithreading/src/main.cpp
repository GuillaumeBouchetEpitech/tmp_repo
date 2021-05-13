
#include "multithreading/Producer.hpp"

#include "utilities/TraceLogger.hpp"

#include <cstdlib> // EXIT_SUCCESS
#include <iostream>

int main()
{
    D_MYLOG("start");

    {
        multithreading::Producer producer;

        producer.initialise(1);

        producer.push([]()
        {
            D_MYLOG("thread 1");
        });

        producer.push([]()
        {
            D_MYLOG("thread 2");
        });

        D_MYLOG("main waiting");

        producer.waitUntilAllCompleted();

        D_MYLOG("main done");
    }

    D_MYLOG("stop");

    return EXIT_SUCCESS;
}
