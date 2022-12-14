#include <iostream>
#include <basic/locale_init.h>
#include <UnitTest++/UnitTest++.h>
#include <db/Renewable/DbConnection.h>
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

	try {
		auto conn = std::make_shared<DbConnection>();
		conn->Connect("127.0.0.1", 3306, "test_user", "12345", "testdb");

		auto stmt = conn->PrepareQuery("SELECT * FROM orders");
		auto rs = stmt->exec();

		size_t rec_count = rs->getRecordsCount();
		for (int i = 0; i < rec_count; i++) {
			rs->gotoRecord(i);
			bool is_null;
			Tcout << rs->getInt(0, is_null) << _T(' ');
			Tcout << rs->getString(1) << std::endl;
		}
		Tcin.get();

		rs->reload();
		rec_count = rs->getRecordsCount();
		for (int i = 0; i < rec_count; i++) {
			rs->gotoRecord(i);
			bool is_null;
			Tcout << rs->getInt(0, is_null) << _T(' ');
			Tcout << rs->getString(1) << std::endl;
		}

		//Tcin.get();
		//conn->ExecScalarQuery("UPDATE orders SET name = 'Alice' WHERE id = 3");
	}
	catch (XException &e) {

		Tcerr << e.what() << std::endl;
	}

	//int ret_value = UnitTest::RunAllTests();
	//return ret_value;
	return 0;
}