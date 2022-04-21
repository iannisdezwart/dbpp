#pragma once

#include <iostream>
#include <string>
#include <vector>

struct TestRecord
{
	int id;
	char name[28];
	char address[28];
	int age;

	bool
	operator==(const TestRecord &other)
	const
	{
		return id == other.id &&
			strcmp(name, other.name) == 0 &&
			strcmp(address, other.address) == 0 &&
			age == other.age;
	}

	friend std::ostream &
	operator<<(std::ostream &os, const TestRecord &record)
	{
		os << "TestRecord { id = " << record.id << ", name = "
			<< record.name << ", address = " << record.address
			<< ", age = " << record.age << " }";
		return os;
	}
};

struct TestRecordProjection
{
	int age;

	TestRecordProjection() {}

	TestRecordProjection(const TestRecord &record)
	{
		age = record.age;
	}

	bool
	operator==(const TestRecordProjection &other)
	const
	{
		return age == other.age;
	}

	friend std::ostream &
	operator<<(std::ostream &os, const TestRecordProjection &record)
	{
		os << "TestRecordProjection { age = " << record.age << " }";
		return os;
	}
};

static std::vector<TestRecord> test_records = {
	TestRecord{ 1, "John", "123 Main St.", 25 },
	TestRecord{ 2, "Jane", "456 Main St.", 26 },
	TestRecord{ 3, "Joe", "789 Main St.", 27 },
	TestRecord{ 4, "Jack", "1011 Main St.", 28 },
	TestRecord{ 5, "Jill", "1213 Main St.", 29 },
	TestRecord{ 6, "Jim", "1415 Main St.", 30 },
	TestRecord{ 7, "Jenny", "1617 Main St.", 31 },
	TestRecord{ 8, "Janet", "1819 Main St.", 32 },
	TestRecord{ 9, "Jerry", "1920 Main St.", 33 },
	TestRecord{ 10, "Jared", "1921 Main St.", 34 }
};