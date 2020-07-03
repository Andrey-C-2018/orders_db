#include <UnitTest++/UnitTest++.h>
#include <db_ext/SortingManager.h>

enum {
	COLUMNS_COUNT = 10
};

SUITE(SortingManagerTest_tests) {

	TEST(GetOneColumnOrderingStr) {

		CSortingManager mgr(COLUMNS_COUNT);
		mgr.sortByColumn(1);

		CHECK_EQUAL(mgr.buildOrderingString().str, "1");
	}
}