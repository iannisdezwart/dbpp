#pragma once

#include "test.hpp"
#include "../in-memory-table.hpp"
#include "../on-disk-table.hpp"
#include "test-record.hpp"

namespace in_memory_table_tests
{

static inline dbpp::InMemoryTable<TestRecord>
create_table()
{
	return dbpp::InMemoryTable<TestRecord>(test_records);
}

static inline void
insert_and_read()
{
	dbpp::InMemoryTable<TestRecord> table;

	for (size_t i = 0; i < test_records.size(); i++)
	{
		table.insert(test_records[i]);
		ASSERT_EQ(table.size(), i + 1);
	}

	for (size_t i = 0; i < test_records.size(); i++)
	{
		ASSERT_EQ(table.read(i), test_records[i]);
	}
}

static inline void
clear_table()
{
	auto table = create_table();
	ASSERT_GT(table.size(), 0UL);

	table.clear();
	ASSERT_EQ(table.size(), 0UL);
}

static inline void
filter_into_memory()
{
	auto table = create_table();

	auto filtered_table = table.filter_into_memory(
		[](const TestRecord &record)
		{
			return record.age > 30;
		});

	for (const TestRecord &record : filtered_table)
	{
		ASSERT_GT(record.age, 30);
	}
}

static inline void
filter_into_disk()
{
	auto table = create_table();

	auto filtered_table = table.filter_into_disk(
		[](const TestRecord &record)
		{
			return record.age > 30;
		});

	for (const TestRecord &record : filtered_table)
	{
		ASSERT_GT(record.age, 30);
	}
}

static inline void
filter_map_into_memory()
{
	auto table = create_table();

	auto filtered_table = table.filter_map_into_memory<TestRecordProjection>(
		[](const TestRecord &record)
		{
			return record.age > 30;
		},
		[](const TestRecord &record)
		{
			return TestRecordProjection(record);
		});

	for (const TestRecordProjection &record : filtered_table)
	{
		ASSERT_GT(record.age, 30);
	}
}

static inline void
filter_map_into_disk()
{
	auto table = create_table();

	auto filtered_table = table.filter_map_into_disk<TestRecordProjection>(
		[](const TestRecord &record)
		{
			return record.age > 30;
		},
		[](const TestRecord &record)
		{
			return TestRecordProjection(record);
		});

	for (const TestRecordProjection &record : filtered_table)
	{
		ASSERT_GT(record.age, 30);
	}
}

TEST_SEQUENCE(InMemoryTable, {
	insert_and_read,
	clear_table,
	filter_into_memory,
	filter_into_disk,
	filter_map_into_memory,
	filter_map_into_disk
});

}