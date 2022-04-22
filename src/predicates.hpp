#pragma once

namespace dbpp::predicates
{

/**
 * Returns a boolean indicating whether two given records can be matched into
 * the given `OutRecord` type.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB>
struct IsMatch
{
	bool
	operator()(const InRecordA &a, const InRecordB &b)
	{
		return OutRecord::is_match(a, b);
	}
};

/**
 * Maps two given records into the given `OutRecord` type.
 */
template <typename OutRecord, typename InRecordA, typename InRecordB>
struct Map
{
	OutRecord
	operator()(const InRecordA &a, const InRecordB &b)
	{
		return OutRecord(a, b);
	}
};

}