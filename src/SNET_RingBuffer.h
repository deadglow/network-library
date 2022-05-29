#pragma once
#include <vector>
#include <stdexcept>

template<class T>
class SNET_RingBuffer
{
private:
	std::vector<T> vector;
	int max_capacity;
	int next_index = 0;
	int current_size = 0;
public:
	SNET_RingBuffer(int max_capacity_init)
	{
		max_capacity = max_capacity_init;
		vector.reserve(max_capacity);
		T empty;
		vector.assign(max_capacity, empty);
	};
	
	~SNET_RingBuffer() = default;

	void push_back(T value)
	{
		vector[next_index] = value;
		next_index++;
		if (next_index >= max_capacity)
			next_index -= max_capacity;
		
		if (current_size < max_capacity)
			current_size++;
	};

	T pop_back()
	{
		if (current_size == 0)
			throw "No values to remove.";

		next_index--;
		if (next_index < 0)
			next_index += max_capacity;
		current_size--;
	};

	T get_at_offset(const int offset)
	{
		if (offset < 0)
			throw std::out_of_range("Offset is negative.");
		
		if (offset >= current_size)
			throw std::out_of_range("Offset is larger than size.");

		int index = next_index - offset - 1;

		if (index < 0)
			index += max_capacity;

		return vector[index];
	};

	const int size()
	{
		return current_size;
	};
};