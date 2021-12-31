#include <gtest/gtest.h>
#include "OpenCLHelloWorld.h"

TEST(load_file_as_string_test, given_a_file_when_load_file_as_string_then_load_the_file) {
	string kernel_source = load_file_as_string("test.cl");
	ASSERT_STREQ(kernel_source.c_str(), "foo");
}

TEST(load_file_as_string_test, given_a_file_that_not_exists_when_load_file_as_string_then_throw_exception) {
	try {
		string kernel_source = load_file_as_string("foo");
	} catch (std::exception const&  err) {
		EXPECT_EQ(err.what(), std::string("Error opening kernel."));
	}
}

TEST(sum, given_two_arrays_of_ints_when_sum_then_sum) {
	int A[] = { 1, 2, 3 };
	int B[] = { 1, 2, 3 };
	int C[] = { 0, 0, 0 };

	sum(3, A, B, C);

	ASSERT_EQ(C[0], 2);
	ASSERT_EQ(C[1], 4);
	ASSERT_EQ(C[2], 6);
}


TEST(parallel_sum, given_two_arrays_of_ints_when_sum_parallel_then_sum) {
	int A[] = { 1, 2, 3 };
	int B[] = { 1, 2, 3 };
	int C[] = { 0, 0, 0 };
	string kernel_source = load_file_as_string("kernel.cl");
	parallel_sum( kernel_source.c_str(), 3, A, B, C);

	ASSERT_EQ(C[0], 2);
	ASSERT_EQ(C[1], 4);
	ASSERT_EQ(C[2], 6);
}