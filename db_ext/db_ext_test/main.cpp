#include <iostream>
#include <basic/locale_init.h>
#include <UnitTest++/UnitTest++.h>

TEST(Sanity) {

	CHECK_EQUAL(1, 1);
}

int main() {

	setLocaleToCurrOS_Locale();
	int ret_value = UnitTest::RunAllTests();

	std::cin.get();
	return ret_value;
}