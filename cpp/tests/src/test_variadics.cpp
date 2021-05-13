
#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>
#include <array>

#include <sstream>

template <typename... T>
struct arity
{
    constexpr static int value = sizeof...(T);
};

template <typename First, typename... Args>
auto sum(const First first, const Args... args) -> decltype(first)
{
    const auto values = {first, args...};
    return std::accumulate(values.begin(), values.end(), First{0});
}

template <typename First, typename... Args>
void countArgs(const First first, const Args... args)
{
    const int count = sizeof...(args) + 1;
    std::array<decltype(first), count> list = {first, args...};

    std::stringstream sstr;

    sstr << "list[" << count << "] = ";
    if (count > 0)
    {
        sstr << list[0];
        for (unsigned int ii = 1; ii < list.size(); ++ii)
            sstr << "," << list[ii];
    }

    std::cout << sstr.str() << std::endl;
}

template <typename... Args>
void countArgs2(const Args... args)
{
    const int count = sizeof...(args);

    std::stringstream sstr;

    sstr << "list2[" << count << "] = ";
    ((sstr << args << ","), ...);

    std::cout << sstr.str() << std::endl;
}

template <typename First, typename... Args>
void countArgs3(std::ostream& os, const First first, const Args... args)
{
    os << first;

    if constexpr (sizeof...(args) > 0)
    {
        os << ", ";

        // this line will only be instantiated if there are further
        // arguments. if rest... is empty, there will be no call to
        // print_all(os).
        countArgs3(os, args...);
    }
}

// template <typename... Args>
// void countArgs4(const Args... args)
// {
//     const int count = sizeof...(args);

//     std::cout << "[" << count << "]" << std::endl;

//     // for (int ii = 0; ii < count; ++ii)
//     //     std::cout << "=> " << args[ii] << std::endl;

//     for (int arg : args)
//         std::cout << "=> " << arg << std::endl;
// };

template <typename First, typename... Args>
void countArgs4(const First first, const Args... args)
{
    const int count = sizeof...(args) + 1;
    std::array<decltype(first), count> list = {first, args...};

    std::stringstream sstr;

    sstr << "list[" << count << "] = ";
    if (count > 0)
    {
        sstr << list[0];
        for (unsigned int ii = 1; ii < list.size(); ++ii)
            sstr << "," << list[ii];
    }

    std::cout << sstr.str() << std::endl;
}

template <typename First, typename... Args>
struct MyTest
{
public:
    MyTest()
    {
    }

    MyTest(const First first, const Args... args)
    {
        const int count = sizeof...(args) + 1;
        std::array<decltype(first), count> list = {first, args...};

        std::stringstream sstr;

        sstr << "list[" << count << "] = ";
        if (count > 0)
        {
            sstr << list[0];
            for (unsigned int ii = 1; ii < list.size(); ++ii)
                sstr << "," << list[ii];
        }

        std::cout << sstr.str() << std::endl;
    }
};









template <int...Args>
void countArgs5()
{
    const int count = sizeof...(Args);
    std::array<int, count> list = {Args...};

    std::stringstream sstr;

    sstr << "countArgs5 - list[" << count << "] = ";
    if (count > 0)
    {
        sstr << list[0];
        for (unsigned int ii = 1; ii < list.size(); ++ii)
            sstr << "," << list[ii];
    }

    std::cout << sstr.str() << std::endl;
}

//
//
//

template <bool... digits>
struct ReversedBinaryValue
{
};

template <>
struct ReversedBinaryValue<>
{
    static constexpr int value = 0;
};

template <bool first, bool... digits>
struct ReversedBinaryValue<first, digits...>
{
    static constexpr int value = first + 2 * ReversedBinaryValue<digits...>::value;
};

template <bool...digits>
void countArgs6()
{
    int value = ReversedBinaryValue<digits...>::value;

    std::cout << "value=" << value << std::endl;
}

//
//
//

template <typename ... Args>
void countArgs7(Args ... args)
{
    const int count = sizeof...(args);
    std::array<int, count> list = {args...};

    std::stringstream sstr;

    sstr << "countArgs7 - list[" << count << "] = ";
    if (count > 0)
    {
        sstr << list[0];
        for (unsigned int ii = 1; ii < list.size(); ++ii)
            sstr << "," << list[ii];
    }

    std::cout << sstr.str() << std::endl;
}










void test_variadics()
{
    {
        std::cout << "arity<>::value = " << arity<>::value << std::endl;
        std::cout << "arity<char>::value = " << arity<char>::value << std::endl;
        std::cout << "arity<char, int>::value = " << arity<char, int>::value << std::endl;
        std::cout << "arity<char, short, int>::value = " << arity<char, short, int>::value << std::endl;
        std::cout << "arity<char, short, int, std::string>::value = " << arity<char, short, int, std::string>::value << std::endl;

        static_assert(arity<>::value == 0);
        static_assert(arity<char, short, int>::value == 3);
    }

    // {
    //     std::cout << "arity2<>::value = " << arity2<>::value << std::endl;
    //     std::cout << "arity2<int>::value = " << arity2<int>::value << std::endl;

    //     static_assert(arity<>::value == 0);
    //     static_assert(arity<char, short, int>::value == 3);
    // }

    {
        // 15
        std::cout << "sum(1, 2, 3, 4, 5) = " << sum(1, 2, 3, 4, 5) << std::endl;
        // 6
        std::cout << "sum(1, 2, 3) = " << sum(1, 2, 3) << std::endl;
        // 7.2
        std::cout << "sum(1.5, 2.0, 3.7) = " << sum(1.5, 2.0, 3.7) << std::endl;
    }

    {
        std::cout << "######" << std::endl;

        {
            countArgs(1, 2, 3, 4, 5);
            countArgs(1, 2, 3);
            countArgs(1.5, 2.0, 3.7);
        }

        std::cout << "######" << std::endl;

        {
            countArgs2(1, 2, 3, 4, 5);
            countArgs2(1, 2, 3);
            countArgs2(1.5, 2.0, 3.7);
            countArgs2();
            countArgs2(5.0, 5.0f, 666);
        }

        std::cout << "######" << std::endl;

        {
            std::stringstream sstr;
            countArgs3(sstr, 6, 7, 8);
            std::cout << "countArgs3 = " << sstr.str() << std::endl;
        }

        std::cout << "######" << std::endl;

        {
            std::stringstream sstr;
            countArgs4(6, 7, 8);
        }

        std::cout << "######" << std::endl;

        {
            MyTest test(9, 10, 11);

            static_cast<void>(test);
        }

        std::cout << "######" << std::endl;

        {
            countArgs5<1>();
            countArgs5<1, 2, 3>();
            countArgs5<1, 2, 3, 4, 5, 6, 7, 8>();
        }

        std::cout << "######" << std::endl;

        {
            countArgs6<true>();
            countArgs6<false, false, true>();
        }

        std::cout << "######" << std::endl;

        {
            countArgs7(1);
            countArgs7(1, 2, 3);
        }

    }
}
