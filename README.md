In this code we explore possibilities of optimizations of single-core binary search on a large sorted array. Therefore, our implementation is trying to improve code like this:

```cpp
int binarySearch_basic(size_t item_count, const Type arr[], int search)
{
	int l = 0;
	int r = item_count - 1;
	while (l <= r) {
		int m = l + ((r - l) >> 1);
		if (arr[m] == search) return m;
		if (arr[m] < search) l = m + 1;
		else r = m - 1;
	}
	return -1;
}
```

The whole idea is based on the assumption that the array is large; therefore, the data can not fit into L2 cache. The algorithm inevitably leads an L2 cache misses during the function call. If we repeatedly run the code some items are accessed frequently (i.e. the middle item), and they will stay in the CPU cache, but most of the data is accessed randomly, and the program will suffer from DRAM read wait.

## Prefetching

The most straightforward idea is to use prefetching. We access the items on both sides of item `m` at the beginning of each cycle.

```cpp
		int m1 = arr[l + ((r - l) >> 2)];
		int m2 = arr[r - ((r - l) >> 2)];
```

It may bring certain improvement; however, it is not very significant.

## Hiding CPU cache misses

Another technique does more memory accesses per iteration. The CPU then optimize the problematic DRAM memory reads then overlap. Let us start with two items per iteration:

```cpp
int binarySearch_duo(size_t item_count, const Type arr[], int search)
{
	int l = 0;
	int r = item_count - 1;
	while (r - l > 7) {
		int delta = (r - l) / 3;
		int m1 = l + delta; // the position of the first item
		int m2 = r - delta; // the position of the second item

		if (arr[m1] == search) return m1;
		if (arr[m2] == search) return m2;

		if (arr[m1] < search) {
			if (arr[m2] < search) {
				l = m2 + 1;
			}
			else {
				r = m2 - 1;
				l = m1 + 1;
			}
		}
		else {
			r = m1 - 1;
		}
	}

    // the sequential search of the short interval
	for (int i = l; i <= r; i++)
	{
		if (arr[i] == search) return i;
	}

	return -1;
}
```

In order to avoid algorithm pitfalls that come from cutting the interval in three parts we simply do the sequential scan when the remaining interval is too small.

## Conclusion

Using the algorithm reading two items per iteration we obtain a solution that is 25% faster than the original one. Reading three items per iteration improve the original algorithm by 30%.