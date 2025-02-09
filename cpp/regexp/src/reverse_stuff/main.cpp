
#include <cstdio> // <= EXIT_SUCCESS
#include <iostream>
#include <string>
#include <memory>
#include <cstring>

void reverse(char* str)
{
    const int size = strlen(str);
    const int hsize = size / 2;
    for (int index = 0; index < hsize; ++index)
        std::swap(str[index], str[size - 1 - index]);
}

int main()
{
    std::string message = "1234567";
    auto smartBuffer = std::make_unique<char[]>(message.size() + 1);
    std::memset(smartBuffer.get(), 0, message.size() + 1);
    std::memcpy(smartBuffer.get(), message.c_str(), message.size());

    std::cout << "in  -> " << smartBuffer.get() << std::endl;
    reverse(smartBuffer.get());
    std::cout << "out -> " << smartBuffer.get() << std::endl;

    return EXIT_SUCCESS;
}
