This code we explore wierd behaviour of MSVC compiler on a binary-search algorithm. We have two codes like this:

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


```cpp
	int binarySearch_switched(size_t item_count, const Type arr[], int search)
	{
		int l = 0;
		int r = item_count - 1;
		while (l <= r) {
			int m = l + ((r - l) >> 1);
			if (arr[m] < search) l = m + 1;
			else if (arr[m] == search) return m;
			else r = m - 1;
		}
		return -1;
	}
```

The second variant is about 20% faster on my laptop using MSVC. We do not observe this on g++.