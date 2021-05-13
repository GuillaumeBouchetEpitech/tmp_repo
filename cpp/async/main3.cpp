

// -> https://solarianprogrammer.com/2013/02/04/sorting-data-in-parallel-cpu-gpu/


// #include <iostream>
// #include <iomanip>
// #include <vector>
// #include <random>
// #include <chrono>
// #include <algorithm>

// #include <thread>
// #include <mutex>
// #include <condition_variable>

// #include <ctime>
// // #include "utils.h"

// #include <future> // async


// #define D_USE_THREADS


// namespace
// {
//     struct t_producer
//     {
//     private:
//         std::thread m_thread;
//         std::mutex m_mutex;
//         std::condition_variable m_cond_var;
//         std::condition_variable m_cond_var_wait;
//         std::function<void()> m_callback{nullptr};
//         bool m_done{false};
//         bool m_notified{false};

//     public:
//         t_producer()
//         {
//             m_thread = std::thread([this]()
//             {
//                 std::unique_lock<std::mutex> lock(m_mutex);
//                 while (!m_done)
//                 {
//                     while (!m_notified) // loop to avoid spurious wakeups
//                         m_cond_var.wait(lock);

//                     // std::this_thread::sleep_for(std::chrono::microseconds(2));

//                     if (m_callback)
//                         m_callback();

//                     m_notified = false;
//                     m_callback = nullptr;

//                     m_cond_var_wait.notify_one();
//                 }
//             });
//         }

//         void start(const std::function<void()>& in_callback)
//         {
//             std::unique_lock<std::mutex> lock(m_mutex);

//             m_callback = in_callback;
//             m_notified = true;
//             m_cond_var.notify_one();
//         }

//         void quit()
//         {
//             if (m_done)
//                 return;

//             {
//                 std::unique_lock<std::mutex> lock(m_mutex);

//                 m_callback = nullptr;
//                 m_notified = true;
//                 m_done = true;
//                 m_cond_var.notify_one();
//             }

//             // unlocked join
//             m_thread.join();
//         }
//     };

//     struct t_consumer
//     {
//     private:
//         std::thread m_thread;
//         std::mutex m_mutex;
//         std::condition_variable m_cond_var;
//         std::condition_variable m_cond_var_wait;
//         std::function<void()> m_callback{nullptr};
//         bool m_done{false};
//         bool m_notified{false};

//     public:
//         t_consumer()
//         {
//             m_thread = std::thread([this]()
//             {
//                 std::unique_lock<std::mutex> lock(m_mutex);
//                 while (!m_done)
//                 {
//                     while (!m_notified) // loop to avoid spurious wakeups
//                         m_cond_var.wait(lock);

//                     // std::this_thread::sleep_for(std::chrono::microseconds(2));

//                     if (m_callback)
//                         m_callback();

//                     m_notified = false;
//                     m_callback = nullptr;

//                     m_cond_var_wait.notify_one();
//                 }
//             });
//         }

//         void start(const std::function<void()>& in_callback)
//         {
//             std::unique_lock<std::mutex> lock(m_mutex);

//             m_callback = in_callback;
//             m_notified = true;
//             m_cond_var.notify_one();
//         }

//         inline void wait()
//         {
//             std::unique_lock<std::mutex> lock(m_mutex);
//             while (m_notified)
//             {
//                 m_cond_var_wait(lock);
//                 std::this_thread::sleep_for(std::chrono::microseconds(1));
//             }
//         }

//         void quit()
//         {
//             if (m_done)
//                 return;

//             {
//                 std::unique_lock<std::mutex> lock(m_mutex);

//                 m_callback = nullptr;
//                 m_notified = true;
//                 m_done = true;
//                 m_cond_var.notify_one();
//             }

//             // unlocked join
//             m_thread.join();
//         }

//         inline bool isDone() const { return m_done; }
//         inline bool isNotified() const { return m_notified; }
//     };


//     const unsigned int pool_size = 8;
//     std::vector<t_consumer*>    pool;

//     void pool_init()
//     {
//         pool.reserve(pool_size);
//         for (unsigned int i = 0; i < pool_size; ++i)
//             pool.push_back(new t_consumer);
//     }

//     void pool_push_task(const std::function<void()>& in_callback)
//     {
//         bool found_one = false;
//         for (auto* pWorker : pool)
//             if (!pWorker->isDone() &&
//                 !pWorker->isNotified())
//             {
//                 pWorker->start(in_callback);
//                 found_one = true;
//                 break;
//             }

//         if (!found_one)
//             pool.front()->start(in_callback);
//     }

//     void pool_wait_all()
//     {
//         for (auto* pWorker : pool)
//             pWorker->wait();
//     }

//     void pool_quit_all()
//     {
//         for (auto* pWorker : pool)
//             pWorker->quit();
//     }
// };




// int main()
// {
//     // pool_init();

//     // std::cout << "push" << std::endl;

//     // pool_push_task([]()
//     // {
//     //     std::cout << "HELLO 1" << std::endl;
//     // });

//     // pool_push_task([]()
//     // {
//     //     std::cout << "HELLO 2" << std::endl;
//     // });

//     // std::cout << "wait" << std::endl;
//     // pool_wait_all();
//     // std::cout << "waited" << std::endl;

//     // // pool_quit_all();
//     // // std::cout << "quited" << std::endl;

//     return 0;
// }







#include <iostream>

#include "threading/Consumer.hpp"
#include "threading/Producer.hpp"


int main()
{
    /**
    Consumer C;

    for (int i = 1; i <= 5; ++i)
    {
        while (!C.isAvailable())
           std::this_thread::sleep_for(std::chrono::milliseconds(1));

        C.execute([i]()
        {
            std::cout << "hello " << i << std::endl;
        });
    }

    while (!C.isAvailable())
       std::this_thread::sleep_for(std::chrono::milliseconds(1));

    C.quit();
    //*/

    /**/
    Producer    P;

    for (int i = 0; i < 10; ++i)
    {
        P.push([i]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            std::cout << "task " << i << std::endl;
        }, [i]()
        {
            std::cout << "task " << i << " completed" << std::endl;
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    P.update();

    P.quit();
    //*/

    return 0;
}