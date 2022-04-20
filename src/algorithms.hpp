#pragma once

#include <functional>

#include "in-memory-table.hpp"
#include "on-disk-table.hpp"

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
template <typename OutRecord, typename InRecordA, typename InRecordB>
OnDiskTable<OutRecord>
bnl_join_into_disk(const OnDiskTable<InRecordA> &a,
	const OnDiskTable<InRecordB> &b,
	std::function<bool(const InRecordA &, const InRecordB &)> filter,
	std::function<OutRecord(const InRecordA &, const InRecordB &)> map)
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

};