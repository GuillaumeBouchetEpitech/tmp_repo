

// -> https://solarianprogrammer.com/2013/02/04/sorting-data-in-parallel-cpu-gpu/


#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <ctime>
// #include "utils.h"

#include <future> // async


// #define D_USE_THREADS


namespace
{
    struct t_consumer
    {
    private:
        std::thread m_thread;
        std::mutex m_mutex;
        std::condition_variable m_cond_var;
        std::condition_variable m_cond_var_wait;
        std::function<void()> m_callback{nullptr};
        bool m_done{false};
        bool m_notified{false};

    public:
        t_consumer()
        {
            m_thread = std::thread([this]()
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                while (!m_done)
                {
                    while (!m_notified) // loop to avoid spurious wakeups
                        m_cond_var.wait(lock);

                    // std::this_thread::sleep_for(std::chrono::microseconds(2));

                    if (m_callback)
                        m_callback();

                    m_notified = false;
                    m_callback = nullptr;

                    m_cond_var_wait.notify_one();
                }
            });
        }

        void start(const std::function<void()>& in_callback)
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_callback = in_callback;
            m_notified = true;
            m_cond_var.notify_one();
        }

        inline void wait()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (m_notified)
            {
                m_cond_var_wait(lock);
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        }

        void quit()
        {
            if (m_done)
                return;

            {
                std::unique_lock<std::mutex> lock(m_mutex);

                m_callback = nullptr;
                m_notified = true;
                m_done = true;
                m_cond_var.notify_one();
            }

            // unlocked join
            m_thread.join();
        }

        inline bool isDone() const { return m_done; }
        inline bool isNotified() const { return m_notified; }
    };


    const unsigned int pool_size = 8;
    std::vector<t_consumer*>    pool;

    void pool_init()
    {
        pool.reserve(pool_size);
        for (unsigned int i = 0; i < pool_size; ++i)
            pool.push_back(new t_consumer);
    }

    void pool_push_task(const std::function<void()>& in_callback)
    {
        bool found_one = false;
        for (auto* pWorker : pool)
            if (!pWorker->isDone() &&
                !pWorker->isNotified())
            {
                pWorker->start(in_callback);
                found_one = true;
                break;
            }

        if (!found_one)
            pool.front()->start(in_callback);
    }

    void pool_wait_all()
    {
        for (auto* pWorker : pool)
            pWorker->wait();
    }

    void pool_quit_all()
    {
        for (auto* pWorker : pool)
            pWorker->quit();
    }
};


//////
//////
//////
//////
//////

//Fill a vector with random numbers in the range [lower, upper]
void rnd_fill(std::vector<double> &V, const double lower, const double upper, const unsigned int seed)
{
    //use the default random engine and an uniform distribution
    std::default_random_engine random_engine(seed);
    std::uniform_real_distribution<double> distribute(lower, upper);

    for (auto &elem : V)
        elem = distribute(random_engine);
}

unsigned int    validate(std::vector<double> &V)
{
    unsigned int i = 1;
    for (;i < V.size(); ++i)
        if (V[i-1] > V[i])
            break;
    return i;
}

//////
//////
//////
//////
//////

//Split "mem" into "parts", e.g. if mem = 10 and parts = 4 you will have: 0,2,4,6,10
//if possible the function will split mem into equal chuncks, if not 
//the last chunck will be slightly larger
std::vector<size_t> bounds(size_t parts, size_t mem)
{
    std::vector<size_t> bnd(parts + 1);
    size_t delta = mem / parts;
    size_t reminder = mem % parts;
    size_t N1 = 0, N2 = 0;
    bnd[0] = N1;
    for (size_t i = 0; i < parts; ++i)
    {
        N2 = N1 + delta;
        if (i == parts - 1)
           N2 += reminder;
        bnd[i + 1] = N2;
        N1 = N2;
    }
    return bnd;
}

//Use std::sort
template <typename RAIter>
void test_sort(const RAIter& in_beg, const RAIter& in_end)
{
    std::sort(in_beg, in_end);
}

//Merge V[n0:n1] with V[n1:n3]. The result is put back to V[n0:n3]
template <typename RAIter>
void par_merge(const RAIter& n0, const RAIter& n1, const RAIter& n2)
{
    std::inplace_merge(n0, n1, n2);
}

template <typename RAIter>
double run_tests_thread(const RAIter& in_beg, const RAIter& in_end)
{
    auto start = std::chrono::steady_clock::now();

    //
    //

    unsigned int parts = 8;

    //

    // int len =  std::distance(in_beg, in_end);
    int len =  (in_end - in_beg);

    //

    //Split the data in "parts" pieces and sort each piece in a separate thread
    std::vector<size_t> bnd = bounds(parts, len);

#ifdef D_USE_THREADS

    std::vector<std::thread> thr;

    //Launch "parts" threads
    thr.reserve(parts);
    for (size_t i = 0; i < parts; ++i)
        thr.push_back(std::thread(test_sort<RAIter>, in_beg+bnd[i], in_beg+bnd[i + 1]));

    for (auto &t : thr)
        t.join();

#else // D_USE_THREADS

    std::vector< std::future<void> > thr;

    //Launch "parts" threads
    thr.reserve(parts);
    for (size_t i = 0; i < parts; ++i)
        thr.push_back(std::async(std::launch::async, test_sort<RAIter>, in_beg+bnd[i], in_beg+bnd[i + 1]));

    for (auto &t : thr)
        t.get();

#endif


    //Merge data
    while (parts >= 2)
    {
        std::vector<size_t> limits;

#ifdef D_USE_THREADS
        std::vector<std::thread> th;
#else // D_USE_THREADS
        std::vector< std::future<void> > th;
#endif

        th.reserve(parts);
        for (size_t i = 0; i < parts - 1; i += 2)
        {

#ifdef D_USE_THREADS
            th.push_back(std::thread(par_merge<RAIter>, in_beg+bnd[i], in_beg+bnd[i+1], in_beg+bnd[i+2]));
#else // D_USE_THREADS
            th.push_back(std::async(std::launch::async, par_merge<RAIter>, in_beg+bnd[i], in_beg+bnd[i+1], in_beg+bnd[i+2]));
#endif

            size_t naux = limits.size();
            if (naux > 0)
            {
                if (limits[naux - 1] != bnd[i])
                    limits.push_back(bnd[i]);

                limits.push_back(bnd[i + 2]);
            }
            else
            {
                limits.push_back(bnd[i]);
                limits.push_back(bnd[i + 2]);
            } 
        }

#ifdef D_USE_THREADS
        for (auto &t : th)
            t.join();
#else // D_USE_THREADS
        for (auto &t : th)
            t.get();
#endif

        parts /= 2;
        bnd = limits;
    }

    //
    //

    auto end = std::chrono::steady_clock::now();

    return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000000;
}

// Use only std::sort for parts = 1
template <typename RAIter>
double run_tests_std_sort(RAIter in_beg, RAIter in_end)
{
    auto start = std::chrono::steady_clock::now();
    
    std::sort(in_beg, in_end);
    
    auto end = std::chrono::steady_clock::now();
    
    return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000000;
}



int main()
{

    {
        pool_init();

        std::cout << "push" << std::endl;

        pool_push_task([]()
        {
            std::cout << "HELLO 1" << std::endl;
        });

        pool_push_task([]()
        {
            std::cout << "HELLO 2" << std::endl;
        });

        std::cout << "wait" << std::endl;
        pool_wait_all();
        std::cout << "waited" << std::endl;

        // pool_quit_all();
        // std::cout << "quited" << std::endl;

        return 0;
    }


    std::vector<double> VSYNC, VASYNC;
    
    //use the system time to create a random seed
    unsigned int seed = (unsigned int) time(nullptr);
    
    // size_t parts = 8;
    
    size_t step = 10;
    size_t mem = 10000000;
    // size_t mem = 16;

    for (size_t i = 16; i <= mem; i = 2 * step, step *= 1.1)
    {
        //Fill V with random numbers in the range [0,1]:
        VSYNC.resize(i);
        rnd_fill(VSYNC, 0.0, 100.0, seed);
        VASYNC = VSYNC;

        double sync = run_tests_std_sort(VSYNC.begin(), VSYNC.end());
        unsigned int syncv = validate(VSYNC);

        double async = run_tests_thread(VASYNC.begin(), VASYNC.end());
        unsigned int asyncv = validate(VASYNC);

        std::cout << std::setw(8) << i;
        std::cout << std::fixed << "    SYNC(valid=" << (syncv == i) << "): " << sync << "s";
        std::cout << std::fixed << ",   ASYNC(valid=" << (asyncv == i) << "): " << async << "s";
        std::cout << std::fixed << ",   async version is " << sync / async << " faster";
        std::cout << std::endl;
    }

    return 0;
}