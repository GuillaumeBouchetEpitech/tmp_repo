
#include <iostream>

#include <iomanip> // profiling

#include <vector>
#include <algorithm>
#include <numeric>
#include <future>

#include <chrono> // profiling


/**

template <typename RAIter>
int sync_sum(RAIter beg, RAIter end)
{
	int sum = 0;
	for (; beg != end; ++beg)
		sum += *beg;
	return sum;
}

template <typename RAIter>
int async_sum(RAIter beg, RAIter end)
{
	auto len = end - beg;

	if (len < 1000)
		return std::accumulate(beg, end, 0);

	RAIter mid = beg + len/3 * 2;
	auto handle = std::async(std::launch::async, async_sum<RAIter>, mid, end);
	int sum = async_sum(beg, mid);
	return sum + handle.get();
}

void	test_sum()
{
	// unsigned int size = 1000000000;
	unsigned int size = 10000000;
	std::vector<int> v(size, 1);

	{
		auto start = std::chrono::steady_clock::now();

		//

		int sum = sync_sum(v.begin(), v.end());

		//

		auto stop = std::chrono::steady_clock::now();
		// auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

		std::cout << "The sum is " << sum << std::endl;
		std::cout << "sync elapsed=" << std::fixed << static_cast<float>(duration) / 1000000 << std::endl;
	}

	{
		auto start = std::chrono::steady_clock::now();

		//

		int sum = async_sum(v.begin(), v.end());

		//

		auto stop = std::chrono::steady_clock::now();
		// auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

		std::cout << "The sum is " << sum << std::endl;
		std::cout << "async elapsed=" << std::fixed << static_cast<float>(duration) / 1000000 << std::endl;
	}
}

//*/



/**/

template <typename RAIter>
void sync_sort(RAIter beg, RAIter end)
{
	std::sort(beg, end);
}
 
template <typename RAIter>
void async_sort(RAIter beg, RAIter end)
{
	std::sort(beg, end);

	// auto len = end - beg;

	// if (len < 1000)
	// 	return std::accumulate(beg, end, 0);

	// RAIter mid = beg + len/2;
	// auto handle = std::async(std::launch::async, async_sum<RAIter>, mid, end);
	// int sum = async_sum(beg, mid);
	// return sum + handle.get();
}

#include <forward_list>

template <class ForwardIt>
void quicksort(ForwardIt beg, ForwardIt end)
{
	if (beg == end)
		return;

	int len =  std::distance(beg,end);

	if (len < 1000)
	{
		std::sort(beg, end);
		return;
	}

	// auto pivot = *std::next(beg, std::distance(beg,end)/2);
	// ForwardIt middle1 = std::partition(beg, end, [pivot](const auto& em){ return em < pivot; });
	// ForwardIt middle2 = std::partition(middle1, end, [pivot](const auto& em){ return !(pivot < em); });
	int pivot = *std::next(beg, len/2);
	ForwardIt middle1 = std::partition(beg, end, [pivot](int em){ return em < pivot; });

	auto handle = std::async(std::launch::async, quicksort<ForwardIt>, beg, middle1);
	// auto handle = std::async(std::launch::async, [&]()
	// {
	// 	quicksort<ForwardIt>(beg, middle1);
	// });
	ForwardIt middle2 = std::partition(middle1, end, [pivot](int em){ return !(pivot < em); });
	quicksort<ForwardIt>(middle2, end);
	handle.get();

	// quicksort(beg, middle1);
	// quicksort(middle2, end);
}

void	test_sort()
{
	// unsigned int size = 1000000000;
	unsigned int size = 1000000;
	std::vector<int> v(size, 0);

	{
		{ // un-sort
			for (unsigned int i = 0; i < v.size(); ++i)
				v[i] = size - i;
		} // un-sort

		{
			auto start = std::chrono::steady_clock::now();

			//

			sync_sort(v.begin(), v.end());

			//

			auto stop = std::chrono::steady_clock::now();
			// auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

			std::cout << "sync elapsed  = " << std::fixed << static_cast<float>(duration) / 1000000 << std::endl;
		}

		{ // validate
			unsigned int i = 1;
			for (;i < v.size(); ++i)
				if (v[i-1] > v[i])
					break;
			std::cout << "valid = " << (i == v.size()) << std::endl;
		} // validate
	}

	{
		{ // un-sort
			for (unsigned int i = 0; i < v.size(); ++i)
				v[i] = size - i;
		} // un-sort

		{
			auto start = std::chrono::steady_clock::now();

			//

			// async_sort(v.begin(), v.end());
			quicksort(v.begin(), v.end());

			//

			auto stop = std::chrono::steady_clock::now();
			// auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

			std::cout << "async elapsed = " << std::fixed << static_cast<float>(duration) / 1000000 << std::endl;
		}

		{ // validate
			unsigned int i = 1;
			for (;i < v.size(); ++i)
				if (v[i-1] > v[i])
					break;
			std::cout << "valid = " << (i == v.size()) << std::endl;
		} // validate
	}
}

//*/





int main()
{
	// test_sum();
	test_sort();

	return 0;
}

