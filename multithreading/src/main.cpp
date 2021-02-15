
#include "multithreading/Producer.hpp"

#include <cstdlib> // EXIT_SUCCESS
#include <iostream>

int main()
{
    std::cout << "start" << std::endl;

    {
        multithreading::Producer producer(1);

        producer.push([]()
        {
            std::cout << "thread 1" << std::endl;
        });

        producer.push([]()
        {
            std::cout << "thread 2" << std::endl;
        });

        std::cout << "main waiting" << std::endl;

        producer.waitUntilAllCompleted();

        std::cout << "main done" << std::endl;
    }

    std::cout << "stop" << std::endl;

    return EXIT_SUCCESS;
}
