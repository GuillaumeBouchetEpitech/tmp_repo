
#include "t_vertex3.hpp"

#include <iostream>

template<typename T>
class custom_vector
{
private:
    std::size_t _capacity = 0;
    std::size_t _size = 0;
    T* _data = nullptr;

public:
    custom_vector() = default;
    ~custom_vector()
    {
        // will call the items' destructor
        clear();

        // this does not call the items' destructor
        ::operator delete(_data, _capacity * sizeof(T));
    }

    void push_back(const T& value)
    {
        if (_size == _capacity)
            _realloc(_capacity * 2);

        _data[_size++] = value;
    }

    void push_back(T&& value)
    {
        if (_size == _capacity)
            _realloc(_capacity * 2);

        _data[_size++] = std::move(value);
    }

    template<typename...Args>
    T& emplace_back(Args&&... args)
    {
        if (_size == _capacity)
            _realloc(_capacity * 2);

        // _data[_size] = T(args...);
        // _data[_size] = T(std::forward<Args>(args)...);
        // new (&_data[_size]) T(args...);

        // this does not allocate, only call the constructor
        new (&_data[_size]) T(std::forward<Args>(args)...);

        return _data[_size++];
    }

    void pop_back()
    {
        if (_size == 0)
            return;

        _data[--_size].~T();
    }

    void clear()
    {
        for (std::size_t ii = 0; ii < _size; ++ii)
            _data[ii].~T();

        _size = 0;
    }

    void reserve(std::size_t capacity)
    {
        _realloc(capacity);
    }

    std::size_t size() const
    {
        return _size;
    }

    std::size_t capacity() const
    {
        return _capacity;
    }

    const T& operator [](std::size_t index) const
    {
        return _data[index];
    }

    T& operator [](std::size_t index)
    {
        return _data[index];
    }

private:
    void _realloc(std::size_t newCapacity)
    {
        if (newCapacity == 0) // true the first time
            newCapacity = 1;

        if (newCapacity < _capacity)
            return;

        // does not allocate, just call the constructor
        T* newData = (T*) ::operator new (newCapacity * sizeof(T));

        for (std::size_t ii = 0; ii < _size; ++ii)
            newData[ii] = std::move(_data[ii]);

        // call the destructor of the moved item(s)
        for (std::size_t ii = 0; ii < _size; ++ii)
            newData[ii].~T();

        // this does not call the items' destructor
        ::operator delete(_data, _capacity * sizeof(T));

        _data = newData;
        _capacity = newCapacity;
    }
};

void test_my_vector()
{
    {
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;

        custom_vector<t_vertex3> vertices;

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

        std::cout << "----------" << std::endl;
    }

    {
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;
        std::cout << "==========" << std::endl;

        custom_vector<t_vertex3> vertices;

        vertices.reserve(8);

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
