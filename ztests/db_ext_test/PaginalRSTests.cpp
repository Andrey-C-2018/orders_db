#include <UnitTest++/UnitTest++.h>
#include <db_ext/PaginalRS.h>
#include "mocks/TestStatement.h"

const int v[] = {11, 1, 22, 2,
				 33, 3, 44, 4,
				 55, 5, 66, 6,
				 77, 7};

SUITE(PaginalRS_tests) {

	TEST(empty) {
		auto stmt = std::make_shared<TestStatement>();
		auto stmt_rec_count = stmt->getRecCountStmt();
		
		PaginalRS rs(stmt, stmt_rec_count, 1);

		CHECK_EQUAL(0, rs.getFieldsCount());
		CHECK_EQUAL(0, rs.getRecordsCount());
		CHECK_EQUAL(2, stmt->bindIntCallsCount());
	}

	TEST(one_rec) {
		const size_t PAGE_SIZE = 5;
		auto stmt = std::make_shared<TestStatement>(v, 2, 1);
		auto stmt_rec_count = stmt->getRecCountStmt();
		
		PaginalRS rs(PAGE_SIZE, stmt, stmt_rec_count, 1);
		CHECK_EQUAL(2, rs.getFieldsCount());
		CHECK_EQUAL(1, rs.getRecordsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(0, stmt->getBindedRecordsOffset());
		stmt->resetBindingsCounter();

		rs.gotoRecord(0);
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
		stmt->resetBindingsCounter();

		bool is_null;
		CHECK_EQUAL(v[0], rs.getInt(0, is_null));
		CHECK(!is_null);
	}

	TEST(all_at_one_page) {
		const size_t PAGE_SIZE = 5;
		auto stmt = std::make_shared<TestStatement>(v, 2, 5);
		auto stmt_rec_count = stmt->getRecCountStmt();
		
		PaginalRS rs(PAGE_SIZE, stmt, stmt_rec_count, 1);
		CHECK_EQUAL(2, rs.getFieldsCount());
		CHECK_EQUAL(5, rs.getRecordsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(0, stmt->getBindedRecordsOffset());
		stmt->resetBindingsCounter();

		rs.gotoRecord(0);
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
		stmt->resetBindingsCounter();

		bool is_null;
		for (size_t i = 0; i < 5; i++) {
			rs.gotoRecord(i);
			CHECK_EQUAL(v[2 * i], rs.getInt(0, is_null));
			CHECK(!is_null);
			CHECK_EQUAL(v[2 * i + 1], rs.getInt(1, is_null));
			CHECK(!is_null);
		}
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
	}

	TEST(not_all_at_one_page) {
		const size_t PAGE_SIZE = 4;
		auto stmt = std::make_shared<TestStatement>(v, 2, 5);
		auto stmt_rec_count = stmt->getRecCountStmt();
		
		PaginalRS rs(PAGE_SIZE, stmt, stmt_rec_count, 1);
		CHECK_EQUAL(2, rs.getFieldsCount());
		CHECK_EQUAL(5, rs.getRecordsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(0, stmt->getBindedRecordsOffset());
		stmt->resetBindingsCounter();

		rs.gotoRecord(0);
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
		stmt->resetBindingsCounter();

		bool is_null;
		for (size_t i = 0; i < PAGE_SIZE; i++) {
			rs.gotoRecord(i);
			CHECK_EQUAL(v[2 * i], rs.getInt(0, is_null));
			CHECK(!is_null);
			CHECK_EQUAL(v[2 * i + 1], rs.getInt(1, is_null));
			CHECK(!is_null);
		}
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
	}

	TEST(page_changing) {
		const size_t PAGE_SIZE = 4;
		auto stmt = std::make_shared<TestStatement>(v, 2, 7);
		auto stmt_rec_count = stmt->getRecCountStmt();
		
		PaginalRS rs(PAGE_SIZE, stmt, stmt_rec_count, 1);
		CHECK_EQUAL(2, rs.getFieldsCount());
		CHECK_EQUAL(7, rs.getRecordsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(0, stmt->getBindedRecordsOffset());
		stmt->resetBindingsCounter();

		rs.gotoRecord(3);
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
		stmt->resetBindingsCounter();

		rs.gotoRecord(4);
		CHECK_EQUAL(2, stmt->bindIntCallsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(4, stmt->getBindedRecordsOffset());
		stmt->resetBindingsCounter();

		bool is_null;
		for (size_t i = 4; i < 7; i++) {
			rs.gotoRecord(i);
			CHECK_EQUAL(v[2 * i], rs.getInt(0, is_null));
			CHECK(!is_null);
			CHECK_EQUAL(v[2 * i + 1], rs.getInt(1, is_null));
			CHECK(!is_null);
		}
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
	}

	TEST(deleted_after_read) {
		const size_t PAGE_SIZE = 4;
		auto stmt = std::make_shared<TestStatement>(v, 2, 7);
		auto stmt_rec_count = stmt->getRecCountStmt();
		
		PaginalRS rs(PAGE_SIZE, stmt, stmt_rec_count, 1);
		stmt->resetBindingsCounter();

		rs.gotoRecord(3);
		stmt->deleteRecord(6);
		CHECK_EQUAL(7, rs.getRecordsCount());
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
		stmt->resetBindingsCounter();

		rs.gotoRecord(6);
		CHECK_EQUAL(7, rs.getRecordsCount());
		CHECK_EQUAL(2, stmt->bindIntCallsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedRecordsOffset());
		stmt->resetBindingsCounter();

		bool is_null;
		rs.getInt(0, is_null);
		CHECK(is_null);

		rs.reload();
		CHECK_EQUAL(6, rs.getRecordsCount());
	}

	TEST(inserted_after_read) {

		const size_t PAGE_SIZE = 4;
		auto stmt = std::make_shared<TestStatement>(v, 2, 7);
		auto stmt_rec_count = stmt->getRecCountStmt();

		PaginalRS rs(PAGE_SIZE, stmt, stmt_rec_count, 1);
		stmt->resetBindingsCounter();

		rs.gotoRecord(3);
		const int new_record[] = {123, 8};
		stmt->insertRecord(2, new_record);
		CHECK_EQUAL(0, stmt->bindIntCallsCount());
		CHECK_EQUAL(7, rs.getRecordsCount());
		stmt->resetBindingsCounter();

		rs.gotoRecord(4);
		CHECK_EQUAL(7, rs.getRecordsCount());
		CHECK_EQUAL(2, stmt->bindIntCallsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedRecordsOffset());
		stmt->resetBindingsCounter();

		bool is_null;
		rs.getInt(0, is_null);
		CHECK(is_null);
		rs.getInt(1, is_null);
		CHECK(is_null);

		for (size_t i = 4; i < 6; i++) {
			rs.gotoRecord(i + 1);
			CHECK_EQUAL(v[2 * i], rs.getInt(0, is_null));
			CHECK(!is_null);
			CHECK_EQUAL(v[2 * i + 1], rs.getInt(1, is_null));
			CHECK(!is_null);
		}
		CHECK_EQUAL(0, stmt->bindIntCallsCount());

		CHECK_EQUAL(7, rs.getRecordsCount());
		rs.reload();
		CHECK_EQUAL(8, rs.getRecordsCount());
	}

	TEST(reverse_order) {
		const size_t PAGE_SIZE = 4;
		auto stmt = std::make_shared<TestStatement>(v, 2, 7);
		auto stmt_rec_count = stmt->getRecCountStmt();

		PaginalRS rs(PAGE_SIZE, stmt, stmt_rec_count, 1);
		stmt->resetBindingsCounter();

		rs.gotoRecord(6);
		CHECK_EQUAL(2, stmt->bindIntCallsCount());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedPageSize());
		CHECK_EQUAL(PAGE_SIZE, stmt->getBindedRecordsOffset());
	}
}