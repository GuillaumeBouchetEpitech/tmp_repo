
#include "./test_promises.hpp"
#include "./test_variadics.hpp"
#include "./test_arrays.hpp"
#include "./test_memory.hpp"
#include "./test_regex.hpp"
#include "./test_my_vector.hpp"

#include <vector>
#include <functional>
#include <iostream>

#include <cstdlib> // <= EXIT_SUCCESS

int main()
{
    {
        std::vector<std::pair<std::string, std::function<void()>>> allTests;

        // std::cout << "step" << std::endl;

        // allTests.reserve(16);

        // std::cout << "step" << std::endl;


        // allTests.push_back(std::make_pair("test_promises", test_promises));
        allTests.push_back(std::make_pair("test_variadics", test_variadics));
        // allTests.push_back(std::make_pair("test_memory", test_memory));
        // allTests.push_back(std::make_pair("test_regex", test_regex));
        // allTests.push_back(std::make_pair("test_arrays", test_arrays));
        // allTests.push_back(std::make_pair("test_my_vector", test_my_vector));

        for (auto currentTest : allTests)
        {
            std::cout << std::endl;
            std::cout << "##############################################" << std::endl;
            std::cout << "starting: " << currentTest.first << std::endl;
            std::cout << "##############################################" << std::endl;
            std::cout << std::endl;

            currentTest.second();

            std::cout << std::endl;
            std::cout << "##############################################" << std::endl;
            std::cout << "ending: " << currentTest.first << std::endl;
            std::cout << "##############################################" << std::endl;
            std::cout << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
