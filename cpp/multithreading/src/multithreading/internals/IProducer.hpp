
#pragma once

#include <functional>
#include <memory>

namespace multithreading
{
    using WorkCallback = std::function<void()>;

    class Consumer;

    class IProducer
    {
        // friendship so the consumers can call _notifyWorkDone()
        friend Consumer;

    public:
        virtual ~IProducer() = default;

    protected:
        virtual void _notifyWorkDone(Consumer* consumer) = 0;
    };

};
