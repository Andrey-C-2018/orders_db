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

TEST(Connect) {

	try {
		auto conn = std::make_shared<CMySQLConnection>();
		conn->Connect("127.0.0.1", 3306, "orders_user", "123", "orders");

		auto rs = conn->ExecQuery("SELECT id_center, center_short_name FROM centers ORDER BY center_short_name");
		CHECK_EQUAL(6, rs->getRecordsCount());

		rs->gotoRecord(0);
		auto center_nm = rs->getImmutableString(1);
		CHECK_EQUAL("Бердичів МЦ", center_nm.str);
	}
	catch (XException &e) {

		Tcerr << e.what() << std::endl;
		exit(1);
	}
}

int main() {

	setLocaleToCurrOS_Locale();

	int ret_value = UnitTest::RunAllTests();
	return ret_value;
}