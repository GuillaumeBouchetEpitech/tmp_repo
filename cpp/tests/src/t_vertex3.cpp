
#include "t_vertex3.hpp"

#include <iostream>

t_vertex3::t_vertex3()
{
    std::cout << "[" << this << "] ctor [default]" << std::endl;
}

t_vertex3::t_vertex3(float x, float y, float z)
    : x(x), y(y), z(z)
{
    std::cout << "[" << this << "] ctor [params]" << std::endl;
}

t_vertex3::t_vertex3(const t_vertex3& other)
    : x(other.x), y(other.y), z(other.z)
{
    std::cout << "[" << this << "] ctor [copy]" << std::endl;
}

t_vertex3::t_vertex3(t_vertex3&& other)
    : x(other.x), y(other.y), z(other.z)
{
    std::cout << "[" << this << "] ctor [move]" << std::endl;
}

t_vertex3::~t_vertex3()
{
    std::cout << "[" << this << "] dtor" << std::endl;
}

t_vertex3& t_vertex3::operator=(const t_vertex3& other)
{
    std::cout << "[" << this << "] operator [copy]" << std::endl;

    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
}

t_vertex3& t_vertex3::operator=(t_vertex3&& other)
{
    std::cout << "[" << this << "] operator [move]" << std::endl;

    if (this != &other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    return *this;
}
