#include <chrono>
#include <cstdio>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_RED "\033[31m"
#define ANSI_ORANGE "\033[33m"
#define ANSI_CYAN "\033[36m"
#define ANSI_LIGHT_GREY "\033[90m"
#define ANSI_LIGHT_RED "\033[91m"
#define ANSI_LIGHT_GREEN "\033[92m"
#define ANSI_LIGHT_BLUE "\033[94m"
#define ANSI_LIGHT_MAGENTA "\033[95m"

template <typename T>
static std::string
to_str(const T &obj)
{
	std::stringstream ss;
	ss << obj;
	return ss.str();
}

template <>
std::string
to_str<std::string>(const std::string &str)
{
	return '"' + str + '"';
}

#define ASSERT(condition) assert_impl(condition, #condition, __FILE__, __LINE__)

static inline void
assert_impl(bool condition, const char *condition_str, const char *file,
	int line)
{
	if (condition)
	{
		return;
	}

	fprintf(stderr, ANSI_LIGHT_RED "Assertion failed: " ANSI_ORANGE "%s"
		ANSI_RESET ".\nAt " ANSI_LIGHT_MAGENTA "%s" ANSI_RESET ":"
		ANSI_LIGHT_MAGENTA "%d" ANSI_RESET ".\n",
		condition_str, file, line);
	throw std::runtime_error("Assertion failed");
}

#define ASSERT_EQ(result, expected) assert_eq_impl(result, expected, \
	#result, #expected, __FILE__, __LINE__)

template <typename T>
void
assert_eq_impl(const T &result, const T &expected, const char *result_str,
	const char *expected_str, const char *file, int line)
{
	if (result == expected)
	{
		return;
	}

	fprintf(stderr, ANSI_LIGHT_RED "Assertion failed: " ANSI_ORANGE "%s"
		ANSI_LIGHT_GREY " (result)" ANSI_CYAN " == " ANSI_ORANGE "%s"
		ANSI_LIGHT_GREY " (expected)" ANSI_RESET ".\nResult was: "
		ANSI_LIGHT_MAGENTA "%s" ANSI_RESET ", but expected "
		ANSI_LIGHT_MAGENTA "%s" ANSI_RESET ".\nAt " ANSI_LIGHT_MAGENTA
		"%s" ANSI_RESET ":" ANSI_LIGHT_MAGENTA "%d" ANSI_RESET ".\n",
		result_str, expected_str, to_str(result).c_str(),
		to_str(expected).c_str(), file, line);
	throw std::runtime_error("Assertion failed");
}

#define ASSERT_GT(result, bound) assert_gt_impl(result, bound, \
	#result, #bound, __FILE__, __LINE__)

template <typename T>
void
assert_gt_impl(const T &result, const T &bound, const char *result_str,
	const char *bound_str, const char *file, int line)
{
	if (result > bound)
	{
		return;
	}

	fprintf(stderr, ANSI_LIGHT_RED "Assertion failed: " ANSI_ORANGE "%s"
		ANSI_LIGHT_GREY " (result)" ANSI_CYAN " > " ANSI_ORANGE "%s"
		ANSI_LIGHT_GREY " (bound)" ANSI_RESET ".\nResult was: "
		ANSI_LIGHT_MAGENTA "%s" ANSI_RESET
		", which is not greater than " ANSI_LIGHT_MAGENTA "%s"
		ANSI_RESET ".\nAt " ANSI_LIGHT_MAGENTA "%s" ANSI_RESET ":"
		ANSI_LIGHT_MAGENTA "%d" ANSI_RESET ".\n",
		result_str, bound_str, to_str(result).c_str(),
		to_str(bound).c_str(), file, line);
	throw std::runtime_error("Assertion failed");
}

#define ASSERT_LT(result, bound) assert_lt_impl(result, bound, \
	#result, #bound, __FILE__, __LINE__)

template <typename T>
void
assert_lt_impl(const T &result, const T &bound, const char *result_str,
	const char *bound_str, const char *file, int line)
{
	if (result > bound)
	{
		return;
	}

	fprintf(stderr, ANSI_LIGHT_RED "Assertion failed: " ANSI_ORANGE "%s"
		ANSI_LIGHT_GREY " (result)" ANSI_CYAN " < " ANSI_ORANGE "%s"
		ANSI_LIGHT_GREY " (bound)" ANSI_RESET ".\nResult was: "
		ANSI_LIGHT_MAGENTA "%s" ANSI_RESET
		", which is not less than " ANSI_LIGHT_MAGENTA "%s"
		ANSI_RESET ".\nAt " ANSI_LIGHT_MAGENTA "%s" ANSI_RESET ":"
		ANSI_LIGHT_MAGENTA "%d" ANSI_RESET ".\n",
		result_str, bound_str, to_str(result).c_str(),
		to_str(bound).c_str(), file, line);
	throw std::runtime_error("Assertion failed");
}

struct TestSequence
{
	std::string name;
	std::vector<std::function<void()>> tests;
	std::string test_names;
	size_t passed_tests;
};

#define TEST_SEQUENCE(name, ...) \
	static TestSequence name = { #name, __VA_ARGS__, #__VA_ARGS__ };

#define TEST_MAIN(...) int main() { test_impl(__VA_ARGS__); }

static std::chrono::high_resolution_clock::time_point time_start;

static inline void
start_timer()
{
	time_start = std::chrono::high_resolution_clock::now();
}

static inline void
stop_timer()
{
	auto time_end = std::chrono::high_resolution_clock::now();
	auto time_diff = time_end - time_start;
	size_t time_diff_us = std::chrono::duration_cast<
		std::chrono::microseconds>(time_diff).count();

	if (time_diff_us <= 10000)
	{
		printf(ANSI_LIGHT_MAGENTA "took %zu microseconds"
			ANSI_RESET ".\n", time_diff_us);
	}
	else if (time_diff_us <= 10000000)
	{
		printf(ANSI_LIGHT_MAGENTA "took %.1f milliseconds"
			ANSI_RESET ".\n", (float) time_diff_us / 1E3);
	}
	else
	{
		printf(ANSI_LIGHT_MAGENTA "took %.2f seconds"
			ANSI_RESET ".\n", (float) time_diff_us / 1E6);
	}
}

static inline void
test_single_sequence(TestSequence &sequence)
{
	printf(ANSI_BOLD ANSI_CYAN "[[ %s ]]" ANSI_RESET "\n",
		sequence.name.c_str());

	// `sequence.test_names` is like "{ test1, test2, test3 }".
	// We want to retrieve the individual test names.
	// We do this by transforming the string into "test1, test2, test3, "
	// and then splitting it before the instances of ", ".

	std::string test_names = sequence.test_names.substr(
		2, sequence.test_names.size() - 4) + ", ";

	sequence.passed_tests = 0;
	size_t current_test = 0;

	for (auto test_function : sequence.tests)
	{
		start_timer();

		std::string test_name = test_names.substr(0, test_names.find(", "));
		test_names = test_names.substr(test_name.size() + 2);

		current_test++;

		try
		{
			test_function();
			sequence.passed_tests++;

			printf(ANSI_LIGHT_GREEN "✔ " ANSI_CYAN "[%zu/%zu] "
				ANSI_BOLD ANSI_LIGHT_BLUE "%s" ANSI_RESET "() ",
				current_test, sequence.tests.size(),
				test_name.c_str());
		}
		catch (std::runtime_error err)
		{
			printf(ANSI_LIGHT_RED "✘ " ANSI_CYAN "[%zu/%zu] "
				ANSI_BOLD ANSI_LIGHT_BLUE "%s" ANSI_RESET "() "
				ANSI_RED "failed" ANSI_RESET "; ",
				current_test, sequence.tests.size(),
				test_name.c_str());
		}

		stop_timer();
	}

	if (sequence.passed_tests == sequence.tests.size())
	{
		printf(ANSI_CYAN "=> " ANSI_BOLD "%s" ANSI_RESET ": "
			ANSI_LIGHT_GREEN "All %zu tests passed" ANSI_RESET
			"!\n", sequence.name.c_str(), sequence.tests.size());
	}
	else
	{
		printf(ANSI_CYAN "=> " ANSI_BOLD "%s" ANSI_RESET ": "
			ANSI_LIGHT_RED "%zu/%zu tests passed" ANSI_RESET
			".\n", sequence.name.c_str(), sequence.passed_tests,
			sequence.tests.size());
	}
}

template <typename ...TestFunctions>
void
test_impl(TestFunctions... tests)
{
	size_t total_tests = 0;
	size_t total_passed_tests = 0;

	for (TestSequence test_sequence : { tests... })
	{
		test_single_sequence(test_sequence);

		total_tests += test_sequence.tests.size();
		total_passed_tests += test_sequence.passed_tests;
	}

	if (total_tests == total_passed_tests)
	{
		printf("\n" ANSI_CYAN ANSI_BOLD "Grand total" ANSI_RESET ": "
			ANSI_LIGHT_GREEN "All %zu tests passed" ANSI_RESET
			"!\n", total_tests);
	}
	else
	{
		printf("\n" ANSI_CYAN ANSI_BOLD "Grand total" ANSI_RESET ": "
			ANSI_LIGHT_RED "%zu/%zu tests passed" ANSI_RESET
			".\n", total_passed_tests, total_tests);
	}
}