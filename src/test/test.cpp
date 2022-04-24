#include "on-disk-table-test.hpp"
#include "table-algorithms-test.hpp"
#include "in-memory-table-test.hpp"

TEST_MAIN(
	on_disk_table_tests::OnDiskTable,
	in_memory_table_tests::InMemoryTable,
	table_algorithm_tests::table_algorithms
);