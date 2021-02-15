
#include "t_vertex3.hpp"

#include <iostream>
#include <array>
#include <vector>

void test_arrays()
{
    // {
    //     std::array<int, 3>  test;
    //     // auto* test2 = new std::array<int, 3>();

    //     // auto callback = [](auto* array) {

    //     //     std::cout << array->size() << std::endl;
    //     // };

    //     // callback(test2);
    // }

    {
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;

        std::vector<t_vertex3> vertices;

        t_vertex3 tmp(1, 2, 3);

        for (int ii = 0; ii < 10; ++ii)
        {
            std::cout << std::endl;

            std::size_t prev_capacity = vertices.capacity();

            vertices.push_back(std::move(tmp));

            // std::cout << "---------- " << "(size=" << vertices.size() << ") (capacity " << prev_capacity << " -> " << vertices.capacity() << ")" << std::endl;
            std::cout << "----------";
            std::cout << " (size=" << vertices.size() << ")";
            if (prev_capacity != vertices.capacity())
                std::cout << " (capacity " << prev_capacity << " -> " << vertices.capacity() << ")";
            std::cout << std::endl;
        }
    }

    {
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;

        std::vector<t_vertex3> vertices;

        vertices.reserve(512);

        std::cout << std::endl;

        std::cout << "----" << std::endl;
        t_vertex3 tmpVertex(1, 2, 3);
        std::cout << "----" << std::endl;

        std::cout << std::endl;

        std::cout << "#### PUSH ####" << std::endl;
        vertices.push_back(t_vertex3(1, 2, 3));

        std::cout << "#### PUSH ####" << std::endl;
        vertices.push_back({ 1, 2, 3 });

        std::cout << "#### PUSH ####" << std::endl;
        vertices.push_back(tmpVertex);

        // std::cout << "#### PUSH ####" << std::endl;
        // vertices.push_back(tmpVertex.x, tmpVertex.y, tmpVertex.z);

        std::cout << std::endl;

        std::cout << "#### EMPLACE ####" << std::endl;
        vertices.emplace_back(1, 2, 3);

        std::cout << "#### EMPLACE ####" << std::endl;
        vertices.emplace_back(t_vertex3(1, 2, 3));

        std::cout << "#### EMPLACE ####" << std::endl;
        vertices.emplace_back(tmpVertex);

        std::cout << "#### EMPLACE ####" << std::endl;
        vertices.emplace_back(tmpVertex.x, tmpVertex.y, tmpVertex.z);
    }
}
