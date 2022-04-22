#pragma once

#include "in-memory-table.hpp"
#include "on-disk-table.hpp"
#include "predicates.hpp"

namespace dbpp
{

/**
 * Joins two on-disk tables into disk using a given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The first table.
 * @param b The second table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
OnDiskTable<OutRecord>
bnl_join_into_disk(
	const OnDiskTable<InRecordA> &a,
	const OnDiskTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create a temporary output table.

	OnDiskTable<OutRecord> out = OnDiskTable<OutRecord>::create_temp();

	// Iterate over all rows in the first table in blocks.

	for (const InRecordA &row_a : a)
	{
		// Iterate over all rows in the second table in blocks.

		for (const InRecordB &row_b : b)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Joins two in-memory tables into disk using a given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The first table.
 * @param b The second table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
OnDiskTable<OutRecord>
bnl_join_into_disk(
	const InMemoryTable<InRecordA> &a,
	const InMemoryTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create a temporary output table.

	OnDiskTable<OutRecord> out = OnDiskTable<OutRecord>::create_temp();

	// Iterate over all rows in the first table in blocks.

	for (const InRecordA &row_a : a)
	{
		// Iterate over all rows in the second table in blocks.

		for (const InRecordB &row_b : b)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Joins an in-memory table with an on-disk table into disk using a
 * given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The in-memory table.
 * @param b The on-disk table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
OnDiskTable<OutRecord>
bnl_join_into_disk(
	const InMemoryTable<InRecordA> &a,
	const OnDiskTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create a temporary output table.

	OnDiskTable<OutRecord> out = OnDiskTable<OutRecord>::create_temp();

	// Iterate over all rows in the on-disk table in blocks.

	for (const InRecordB &row_b : b)
	{
		// Iterate over all rows in the in-memory table in blocks.

		for (const InRecordA &row_a : a)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Joins an on-disk table with an in-memory table into disk using a
 * given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The on-disk table.
 * @param b The in-memory table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
OnDiskTable<OutRecord>
bnl_join_into_disk(
	const OnDiskTable<InRecordA> &a,
	const InMemoryTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create a temporary output table.

	OnDiskTable<OutRecord> out = OnDiskTable<OutRecord>::create_temp();

	// Iterate over all rows in the on-disk table in blocks.

	for (const InRecordA &row_a : a)
	{
		// Iterate over all rows in the in-memory table in blocks.

		for (const InRecordB &row_b : b)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Joins two on-disk tables into memory using a given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The first table.
 * @param b The second table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
InMemoryTable<OutRecord>
bnl_join_into_memory(
	const OnDiskTable<InRecordA> &a,
	const OnDiskTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create an output table.

	InMemoryTable<OutRecord> out;

	// Iterate over all rows in the first table in blocks.

	for (const InRecordA &row_a : a)
	{
		// Iterate over all rows in the second table in blocks.

		for (const InRecordB &row_b : b)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Joins two in-memory tables into memory using a given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The first table.
 * @param b The second table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
InMemoryTable<OutRecord>
bnl_join_into_memory(
	const InMemoryTable<InRecordA> &a,
	const InMemoryTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create an output table.

	InMemoryTable<OutRecord> out;

	// Iterate over all rows in the first table in blocks.

	for (const InRecordA &row_a : a)
	{
		// Iterate over all rows in the second table in blocks.

		for (const InRecordB &row_b : b)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Joins an in-memory table with an on-disk table into memory using a
 * given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The in-memory table.
 * @param b The on-disk table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
InMemoryTable<OutRecord>
bnl_join_into_memory(
	const InMemoryTable<InRecordA> &a,
	const OnDiskTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create an output table.

	InMemoryTable<OutRecord> out;

	// Iterate over all rows in the on-disk table in blocks.

	for (const InRecordB &row_b : b)
	{
		// Iterate over all rows in the in-memory table in blocks.

		for (const InRecordA &row_a : a)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Joins an on-disk table with an in-memory table into memory using a
 * given join predicate.
 * Uses the block nested loop join algorithm.
 *
 * @param a The on-disk table.
 * @param b The in-memory table.
 * @param filter The join filter function.
 * @param map The mapping function.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB,
	typename Filter = predicates::IsMatch<OutRecord, InRecordA, InRecordB>,
	typename Map = predicates::Map<OutRecord, InRecordA, InRecordB>>
InMemoryTable<OutRecord>
bnl_join_into_memory(
	const OnDiskTable<InRecordA> &a,
	const InMemoryTable<InRecordB> &b,
	Filter filter = Filter(),
	Map map = Map())
{
	// Create an output table.

	InMemoryTable<OutRecord> out;

	// Iterate over all rows in the on-disk table in blocks.

	for (const InRecordA &row_a : a)
	{
		// Iterate over all rows in the in-memory table in blocks.

		for (const InRecordB &row_b : b)
		{
			// If the join filter function returns true,
			// add the joined row to the output table.

			if (filter(row_a, row_b))
			{
				out.insert(map(row_a, row_b));
			}
		}
	}

	return out;
}

/**
 * Checks whether two in-memory tables have equal contents.
 *
 * @param a The first table.
 * @param b The second table.
 */
template <typename Record>
bool
operator==(const InMemoryTable<Record> &a, const InMemoryTable<Record> &b)
{
	// If the tables have different sizes, they are not equal.

	if (a.size() != b.size())
	{
		return false;
	}

	// Iterate over the rows of both tables.

	auto it1 = a.begin();
	auto it2 = b.begin();

	while (it1 != a.end() && it2 != b.end())
	{
		// If the rows are not equal, the tables are not equal.

		if (!(*it1 == *it2))
		{
			return false;
		}

		// Advance to the next rows.

		++it1;
		++it2;
	}

	// The tables are equal.

	return true;
}

/**
 * Checks whether two on-disk tables have equal contents.
 *
 * @param a The first table.
 * @param b The second table.
 */
template <typename Record>
bool
operator==(const OnDiskTable<Record> &a, const OnDiskTable<Record> &b)
{
	// If the tables have different sizes, they are not equal.

	if (a.size() != b.size())
	{
		return false;
	}

	// Iterate over the rows of both tables.

	auto it1 = a.begin();
	auto it2 = b.begin();

	while (it1 != a.end() && it2 != b.end())
	{
		// If the rows are not equal, the tables are not equal.

		if (!(*it1 == *it2))
		{
			return false;
		}

		// Advance to the next rows.

		++it1;
		++it2;
	}

	// The tables are equal.

	return true;
}

/**
 * Checks whether an in-memory table and an on-disk table have equal contents.
 *
 * @param a The in-memory table.
 * @param b The on-disk table.
 */
template <typename Record>
bool
operator==(const InMemoryTable<Record> &a, const OnDiskTable<Record> &b)
{
	// If the tables have different sizes, they are not equal.

	if (a.size() != b.size())
	{
		return false;
	}

	// Iterate over the rows of both tables.

	auto it1 = a.begin();
	auto it2 = b.begin();

	while (it1 != a.end() && it2 != b.end())
	{
		// If the rows are not equal, the tables are not equal.

		if (!(*it1 == *it2))
		{
			return false;
		}

		// Advance to the next rows.

		++it1;
		++it2;
	}

	// The tables are equal.

	return true;
}

/**
 * Checks whether an on-disk table and an in-memory table have equal contents.
 *
 * @param a The on-disk table.
 * @param b The in-memory table.
 */
template <typename Record>
bool
operator==(const OnDiskTable<Record> &a, const InMemoryTable<Record> &b)
{
	return b == a;
}

};