
#pragma once

#include "IProducer.hpp"
#include "Consumer.hpp"
#include "ThreadSynchroniser.hpp"

#include <list>
#include <vector>
#include <functional>

#include <thread>

namespace multithreading
{
    class Producer
        : public IProducer
    {
    private:

        struct t_task
        {
        public:
            workCallback work = nullptr;
            Consumer* consumer = nullptr;

        public:
            t_task(const workCallback& work);
        };

    private:
        std::thread _thread;
        ThreadSynchroniser _waitOneTask;
        ThreadSynchroniser _waitAllTask;

        bool _running = false;

        std::list<Consumer*> _consumers;
        std::list<Consumer*> _freeConsumers;
        std::list<Consumer*> _busyConsumers;

        std::list<t_task> _plannedTasks;
        std::list<t_task> _runningTasks;

    public:
        Producer(unsigned int totalCores);
        virtual ~Producer();

    public:
        void push(const workCallback& work);
        void quit();
        void waitUntilAllCompleted();

    private:
        virtual void _notifyWorkDone(Consumer* in_consumer) override;
        void _threadedMethod();
    };

};
