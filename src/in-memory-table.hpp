#pragma once

#include <vector>

namespace dbpp
{

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