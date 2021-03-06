#pragma once

#include "test.hpp"
#include "../on-disk-table.hpp"
#include "test-record.hpp"

#define TABLE_PATH "/tmp/test-table"

namespace on_disk_table_tests
{

static inline dbpp::OnDiskTable<TestRecord>
create_table()
{
	auto table = dbpp::OnDiskTable<TestRecord>::open(TABLE_PATH);
	table.clear();

	// Insert the test records.

	for (const TestRecord &test_record : test_records)
	{
		table.insert(test_record);
	}

	return table;
}

static inline void
create_and_delete_table()
{
	// Create a table.

	auto table = dbpp::OnDiskTable<TestRecord>::open(TABLE_PATH);
	table.clear();

	ASSERT(table.rows_file.exists());
	ASSERT(table.rows_write_buffer != nullptr);
	ASSERT(!table.temp);

	// Remove the table.

	table.remove();

	ASSERT(!dbpp::io::file_exists(TABLE_PATH));
}

static inline void
insert_and_read()
{
	auto table = dbpp::OnDiskTable<TestRecord>::open(TABLE_PATH);
	table.clear();

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
flush_write_buffer()
{
	ASSERT_GT(dbpp::OnDiskTable<TestRecord>::ENTRIES_PER_BLOCK, 1UL);

	auto table = dbpp::OnDiskTable<TestRecord>::open(TABLE_PATH);
	table.clear();

	ASSERT_EQ(table.rows_file.size(), 0UL);

	table.insert(test_records[0]);

	ASSERT_EQ(table.rows_file.size(), 0UL);

	table.flush_write_buffer();

	ASSERT_EQ(table.rows_file.size(), sizeof(TestRecord));
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

TEST_SEQUENCE(OnDiskTable, {
	create_and_delete_table,
	insert_and_read,
	clear_table,
	flush_write_buffer,
	filter_into_memory,
	filter_into_disk,
	filter_map_into_memory,
	filter_map_into_disk
});

}