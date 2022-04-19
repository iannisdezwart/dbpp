#pragma once

#include "io.hpp"

namespace dbpp
{

/**
 * Controls the heap of a table.
 * Provides methods to allocate and deallocate items on the heap.
 */
struct HeapController
{
	// Reference to the heap file
	io::File &heap_file;

	/**
	 * Initialises the heap controller given a heap file.
	 */
	HeapController(io::File &heap_file)
		: heap_file(heap_file) {}

	/**
	 * Reads a single item from the heap.
	 */
	template <typename Item>
	Item
	read_item(size_t offset)
	{
		// Read the item from the heap.

		Item item;
		heap_file.read_at(offset, sizeof(Item), (void *) &item);
	}
};

};