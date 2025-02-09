
#include <cstdio> // <= EXIT_SUCCESS
#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <cassert>
#include <stdint.h>


class my_string
{
private:
    std::size_t _size = 0;
    std::unique_ptr<char[]> _data;

public:
    my_string() = default;
    virtual ~my_string() = default;
    my_string(const char* inData)
    {
        _size = strlen(inData);
        _data = std::make_unique<char[]>(_size + 1);
        std::memset(_data.get(), 0, _size + 1);
        std::memcpy(_data.get(), inData, _size);
    }
    my_string(const my_string& other)
    {
        if (this == &other)
            return;
        _size = other._size;
        _data = std::make_unique<char[]>(_size + 1);
        std::memset(_data.get(), 0, _size + 1);
        std::memcpy(_data.get(), other._data.get(), _size);
    }
    my_string(my_string&& other)
    {
        if (this == &other)
            return;
        std::swap(_size, other._size);
        std::swap(_data, other._data);
    }

public:
    my_string& operator=(const my_string& other)
    {
        if (this == &other)
            return *this;

        _size = other._size;
        _data = std::make_unique<char[]>(_size + 1);
        std::memset(_data.get(), 0, _size + 1);
        std::memcpy(_data.get(), other._data.get(), _size);
        return *this;
    }
    my_string& operator=(my_string&& other)
    {
        if (this == &other)
            return *this;
        std::swap(_size, other._size);
        std::swap(_data, other._data);
        return *this;
    }

public:
    bool operator==(const my_string& other) const
    {
        if (_size != other._size)
            return false;
        return strncmp(_data.get(), other._data.get(), _size) == 0;
    }
    bool operator!=(const my_string& other) const
    {
        return !my_string::operator==(other); // reuse
    }

public:
    bool operator==(const char* inData) const
    {
        const std::size_t dataSize = strlen(inData);
        if (_size != dataSize)
            return false;
        return strncmp(_data.get(), inData, _size) == 0;
    }
    bool operator!=(const char* inData) const
    {
        return !my_string::operator==(inData); // reuse
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const my_string& data);
};

std::ostream& operator<<(std::ostream& os, const my_string& data)
{
    os << data._data.get();
    return os;
}

int main()
{

    const char* k_value1 = "LOL";
    const char* k_value2 = "LOOOOOOOOOOOOOOOOOOL";

    my_string myStr1 = k_value1;
    my_string myStr2 = k_value1;
    my_string myStr3 = k_value2;

    assert(myStr1 == k_value1);
    assert(myStr1 != k_value2);
    assert(myStr2 == k_value1);
    assert(myStr2 != k_value2);
    assert(myStr3 == k_value2);
    assert(myStr3 != k_value1);

    assert(myStr1 == myStr2);
    assert(!(myStr1 != myStr2));
    assert(myStr1 != myStr3);
    assert(myStr2 != myStr3);
    assert(!(myStr1 == myStr3));
    assert(!(myStr2 == myStr3));

    myStr2 = myStr3; // copy

    assert(myStr1 == k_value1);
    assert(myStr2 == k_value2); // now is value2
    assert(myStr3 == k_value2);
    assert(myStr2 == myStr3);
    assert(myStr1 != myStr2);
    assert(myStr1 != myStr3);

    myStr1 = std::move(myStr2); // move

    assert(myStr1 == k_value2); // now is value2
    assert(myStr2 == k_value1); // now is value1
    assert(myStr3 == k_value2);
    assert(myStr1 == myStr3);
    assert(myStr2 != myStr1);
    assert(myStr2 != myStr3);

    std::cout << "myStr1  -> " << myStr1 << std::endl;
    std::cout << "myStr2  -> " << myStr2 << std::endl;
    std::cout << "myStr3  -> " << myStr3 << std::endl;
    std::cout << "myStr1 == myStr2 -> " << (myStr1 == myStr2) << std::endl;
    std::cout << "myStr1 != myStr2 -> " << (myStr1 != myStr2) << std::endl;

    return EXIT_SUCCESS;
}
