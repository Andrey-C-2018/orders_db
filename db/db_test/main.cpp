#include <iostream>
#include <basic/locale_init.h>
#include <UnitTest++/UnitTest++.h>
#include <db/MySQL/MySQLConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>
#include <basic/tstream.h>

TEST(Sanity) {

	CHECK_EQUAL(1, 1);
}

int main() {

	setLocaleToCurrOS_Locale();

	int ret_value = UnitTest::RunAllTests();
	return ret_value;
}