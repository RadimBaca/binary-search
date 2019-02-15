
#include <chrono>
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

constexpr int64_t line_count = 15000000;
constexpr size_t CACHE_LINE_SIZE = 64;
using Type = int64_t;
constexpr int64_t test_size= 500000;

//#define TEST_RESULT_CORRECTNESS


template <size_t item_count>
Type init_memory(Type* memory)
{
	Type max = 0;

	for (auto i = 0; i < item_count; i++)
	{
		memory[i] = i;

		max = i > max ? i : max;
	}

	return max;
}



int binarySearch_basic(size_t item_count, const Type arr[], int search)
{
	int l = 0;
	int r = item_count - 1;
	while (l <= r) {

		int m = l + ((r - l) >> 1);

		if (arr[m] == search)
		{
			return m;
		}

		if (arr[m] < search)
		{
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}
	}

	return -1;
}


int binarySearch_reordered(size_t item_count, const Type arr[], int search)
{
	int l = 0;
	int r = item_count - 1;
	while (l <= r) {

		int m = l + ((r - l) >> 1);

		if (arr[m] < search)
		{
			l = m + 1;
		}
		else {
			if (arr[m] == search) {
				return m;
			}
			else {
				r = m - 1;
			}
		}
	}

	return -1;
}


int main()
{
	constexpr size_t vector_size = CACHE_LINE_SIZE * line_count;
	constexpr size_t item_count = vector_size / sizeof(Type);

	std::cerr << "allocating data" << std::endl;
#ifdef _MSC_VER
	Type* memory = (Type*)_aligned_malloc(vector_size, 64);
#else
	if (posix_memalign((void**)&memory, 64, vector_size))
	{
		std::cerr << "Couldn't allocate memory" << std::endl;
		return 1;
	}
#endif

	const auto max = init_memory<item_count>(memory);

	// testing data
	std::cerr << "creating testing data" << std::endl;
	std::vector<Type> test;
	srand(1000);
	for (auto i = 0; i < (test_size); i++)
	{
		test.push_back(((rand() << 16) + (rand() << 8) + rand()) % max);
	}

	std::cerr << "main test started" << std::endl;

#ifdef TEST_RESULT_CORRECTNESS
	std::vector<int> result1;
	std::vector<int> result2;
	std::vector<int> result3;
	result1.reserve(test.size());
	result2.reserve(test.size());
	result3.reserve(test.size());
#endif

	int(*f[2])(size_t, const Type*, int) = { &binarySearch_basic, &binarySearch_reordered };
	std::vector<std::string> algo = { "Basic", "Reordered" };

	for (auto fn = 0; fn < 2; fn++)
	{
		auto ff = f[fn];
		const auto t1 = std::chrono::steady_clock::now();
		for (uint64_t k = 0; k < 10; k++)
		{
			for (int i = 0; i < test_size; i++)
			{
#ifdef TEST_RESULT_CORRECTNESS
				result1.push_back(binarySearch_basic(item_count, memory, test[i]));
#else
				ff(item_count, memory, test[i]);
#endif
			}
		}
		const auto t2 = std::chrono::steady_clock::now();
		std::cerr << algo[fn] << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
	}


#ifdef TEST_RESULT_CORRECTNESS
	int iteration = 0;
	for (auto i = result1.begin(), j = result2.begin(), k = result3.begin(); 
		i != result1.end() && j != result2.end() && k != result3.end(); 
		i++, j++, k++)
	{
		iteration++;
		if (*i != *j || *i != *k)
		{
			std::cerr << "Different result: " << std::to_string(iteration) << std::endl;
		}
	}
#endif


#ifdef _MSC_VER
	_aligned_free(memory);
#else
	free(memory);
#endif

	return 0;
}
