#pragma once

#include <concepts>
#include <string>
#include <vector>

namespace dbpp
{

template <typename Record>
struct OnDiskTable;

/**
 * Represents an in-memory stored table.
 * Provides methods to access content of the table.
 */
template <typename Record>
struct InMemoryTable
{
	// Holds the rows of this table.
	std::vector<Record> rows;

	/**
	 * Constructs an `InMemoryTable` object.
	 */
	InMemoryTable() {}

	/**
	 * Constructs an `InMemoryTable` object with the given rows.
	 */
	InMemoryTable(const std::vector<Record> &rows)
		: rows(rows) {}

	/**
	 * Move constructor.
	 */
	InMemoryTable(InMemoryTable &&other)
		: rows(std::move(other.rows)) {}

	/**
	 * Converts the table to a human-readable string.
	 */
	friend std::ostream &
	operator<<(std::ostream &os, const InMemoryTable &table)
	{
		os << std::string("InMemoryTable { rows = [ ");

		for (const Record &record : table.rows)
		{
			os << record << ", ";
		}

		os << std::string("] }");

		return os;
	}

	/**
	 * Returns the number of records in this table.
	 */
	size_t
	size()
	const
	{
		return rows.size();
	}

	/**
	 * Clears the table, removing all rows.
	 */
	void
	clear()
	{
		rows.clear();
	}

	/**
	 * Inserts a new row into the table.
	 */
	void
	insert(const Record &row)
	{
		rows.push_back(row);
	}

	/**
	 * Inserts a new row into the table using move semantics.
	 */
	void
	insert(Record &&row)
	{
		rows.push_back(std::move(row));
	}

	/**
	 * Reads an entry from the table at the given index.
	 */
	Record &
	read(size_t index)
	{
		return rows[index];
	}

	/**
	 * Filters the table using the given predicate.
	 *
	 * @param filter The filter function to use.
	 * @returns An `InMemoryTable` containing the filtered rows.
	 */
	template <typename Filter>
	InMemoryTable<Record>
	filter_into_memory(Filter filter)
	{
		InMemoryTable<Record> table;

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				table.insert(record);
			}
		}

		return table;
	}

	/**
	 * Filters the table using the given predicate.
	 *
	 * @param filter The filter function to use.
	 * @returns An `OnDiskTable` containing the filtered rows.
	 */
	template <typename Filter, typename OnDiskTableOfRecord = OnDiskTable<Record>>
	OnDiskTableOfRecord
	filter_into_disk(Filter filter)
	{
		OnDiskTableOfRecord table = OnDiskTableOfRecord::create_temp();

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				table.insert(record);
			}
		}

		return table;
	}

	/**
	 * Filters the table using the given predicate.
	 * Then maps the rows using the given function.
	 *
	 * @param filter The filter function to use.
	 * @param map The map function to use.
	 * @returns An `InMemoryTable` containing the filter-mapped rows.
	 */
	template <typename OutRecord, typename Filter, typename Map>
	InMemoryTable<OutRecord>
	filter_map_into_memory(Filter filter, Map map)
	{
		InMemoryTable<OutRecord> table;

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				// Map the rows using the mapping function.

				table.insert(map(record));
			}
		}

		return table;
	}

	/**
	 * Filters the table using the given predicate.
	 * Then maps the rows using the given function.
	 *
	 * @param filter The filter function to use.
	 * @param map The map function to use.
	 * @returns An `OnDiskTable` containing the filter-mapped rows.
	 */
	template <typename OutRecord, typename Filter, typename Map,
		typename OnDiskTableOfOutRecord = OnDiskTable<OutRecord>>
	OnDiskTableOfOutRecord
	filter_map_into_disk(Filter filter, Map map)
	{
		auto table = OnDiskTableOfOutRecord::create_temp();

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				// Map the rows using the mapping function.

				table.insert(map(record));
			}
		}

		return table;
	}

	/**
	 * Returns an iterator that points to the first record of the table.
	 */
	typename std::vector<Record>::iterator
	begin()
	{
		return rows.begin();
	}

	/**
	 * Returns an iterator that points to the end of the table.
	 */
	typename std::vector<Record>::iterator
	end()
	{
		return rows.end();
	}

	/**
	 * Returns a read-only iterator that points to the first record of
	 * the table.
	 */
	typename std::vector<Record>::const_iterator
	begin()
	const
	{
		return rows.begin();
	}

	/**
	 * Returns a read-only iterator that points to the end of the table.
	 */
	typename std::vector<Record>::const_iterator
	end()
	const
	{
		return rows.end();
	}
};

}