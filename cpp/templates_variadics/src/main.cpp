
#include <iostream>
#include <cstdio>

#include <array>


template<typename ... Args>
void test(Args ... args)
{
    std::array<std::string, sizeof...(args)> my_array{{ args... }};

    std::cout << my_array.size() << std::endl;
    for (auto& val : my_array)
    {
        std::cout << " => " << val << std::endl;
    }
}


struct Field
{
    std::string name;
    std::string value;

    Field(const std::string& name, const std::string& value)
        : name(name)
        , value(value)
    {}
};

template<typename ... Args>
void test2(Args ... args)
{
    std::array<Field, sizeof...(args)> my_array{{ args... }};

    std::cout << my_array.size() << std::endl;
    for (Field& val : my_array)
    {
        std::cout << " => " << val.name << ", " << val.value << std::endl;
    }
}


int main()
{
    test("test");
    test("testA", "testB");

    test2(Field("lol", "mdr"), Field("lol", "mdr"));

    return EXIT_SUCCESS;
}

