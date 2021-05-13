
#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>

void test_promises()
{
    {
        // Demonstrate using promise<int> to transmit a result between threads.

        auto accumulateCallback = [](std::vector<int> numbers,
                                     std::promise<int> accumulate_promise) {

            int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
            accumulate_promise.set_value(sum);  // Notify future
        };

        std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
        std::promise<int> accumulate_promise;
        std::future<int> accumulate_future = accumulate_promise.get_future();
        std::thread work_thread(accumulateCallback, numbers, std::move(accumulate_promise));

        // future::get() will wait until the future has a valid result and retrieves it.
        // Calling wait() before get() is not needed
        //accumulate_future.wait();  // wait for result
        std::cout << "result=" << accumulate_future.get() << std::endl;
        work_thread.join();  // wait for thread completion
    }

    {
        // Demonstrate using promise<void> to signal state between threads.

        auto doWorkCallback = [](std::promise<void> barrier_promise) {

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            barrier_promise.set_value();
        };

        std::promise<void> barrier_promise;
        std::future<void> barrier_future = barrier_promise.get_future();
        std::thread new_work_thread(doWorkCallback, std::move(barrier_promise));
        barrier_future.wait();
        new_work_thread.join();
    }
}
