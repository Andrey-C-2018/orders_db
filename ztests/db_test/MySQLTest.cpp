#include <memory>
#include <basic/tstream.h>
#include <UnitTest++/UnitTest++.h>
#include <db/MySQL/MySQLConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>
#include <db/TransactionGuard.h>

// CREATE DATABASE IF NOT EXISTS test_db COLLATE utf8_unicode_ci;
// CREATE USER 'test_user'@'localhost' IDENTIFIED BY '12345';
// GRANT ALL PRIVILEGES ON test_db.* TO 'test_user'@'localhost';

class CMySQLTest {
	static std::shared_ptr<CMySQLConnection> conn;
	std::string query;

protected:
	
	inline void connect() {
	
		if (!conn) {
				
			try {
				conn = std::make_shared<CMySQLConnection>();
				conn->Connect("localhost", 3306, "test_user", "12345", "test_db");
			}
			catch (XException &e) {

				Tcerr << e.what() << std::endl;
				exit(1);
			}
		}
	}

	std::shared_ptr<CMySQLConnection> getConn() {

		return conn;
	}

	void dropTable(const char *table_name) {

		query = "DROP TABLE IF EXISTS ";
		query += table_name;

		try {
			conn->ExecScalarQuery(query.c_str());
		}
		catch (XException &e) {

			Tcerr << _T("Cannot drop table ") << table_name << _T(": ");
			Tcerr << e.what() << std::endl;
		}
	}

	void createTableOrders() {

		query = "CREATE TABLE orders(id_center_legalaid INT(4) NOT NULL,";
		query += " id INT(6) NOT NULL, order_date DATE NOT NULL, id_adv INT(6) NOT NULL,";
		query += " client_name VARCHAR(255), client_bdate DATE,";
		query += " order_type ENUM('КЗ', 'ЗЗП', 'АДМ', 'ZZP'), fee DECIMAL(8,2), ";
		query += " PRIMARY KEY(id_center_legalaid, id, order_date))";

		try {
			conn->ExecScalarQuery(query.c_str());
		}
		catch (XException &e) {

			Tcerr << _T("Cannot create table orders: ") << e.what() << std::endl;
		}
	}

	record_t insertIntoOrders(const char *id_center, const char *id, \
								const char *order_date, const char *id_adv, \
								const char *client_name, const char *bdate, \
								const char *order_type, const char *fee) {

		query = "INSERT INTO orders VALUES (";
		query += id_center;
		query += ", ";
		query += id;
		query += ", '";
		query += order_date;
		query += "', ";
		query += id_adv;
		query += ", '";
		query += client_name;
		query += "', ";
		if (bdate) {
			query += "'";
			query += bdate;
			query += "'";
		}
		else
			query += "NULL";
		query += ", '";
		query += order_type;
		query += "', ";
		query += fee;
		query += ')';

		record_t inserted_recs_count = 0;
		try {
			inserted_recs_count = conn->ExecScalarQuery(query.c_str());
		}
		catch (XException &e) {

			Tcerr << _T("Cannot insert a record into table orders: ");
			Tcerr << e.what() << std::endl;
		}

		return inserted_recs_count;
	}

	std::shared_ptr<IDbStatement> getInsertionStmtOrders() {

		std::shared_ptr<IDbStatement> stmt;
		try {
			stmt = conn->PrepareQuery("INSERT INTO orders VALUES (?,?,?,?,?,?,?,?)");
		}
		catch (XException &e) {

			Tcerr << _T("Error during the orders table insertion stmt preparation: ");
			Tcerr << e.what() << std::endl;
		}

		return stmt;
	}

	record_t insertIntoOrders(std::shared_ptr<IDbStatement> stmt, \
							const int id_center, const int id, \
							CDate order_date, const int id_adv, \
							const char *client_name, CDate bdate, \
							const char *order_type,	const char *fee) {

		if (!stmt) return 0;

		record_t inserted_recs_count = 0;
		try {
			stmt->bindValue(0, id_center);
			stmt->bindValue(1, id);
			stmt->bindValue(2, order_date);
			stmt->bindValue(3, id_adv);
			stmt->bindValue(4, client_name);
			if (bdate.isNull())
				stmt->bindNull(5);
			else
				stmt->bindValue(5, bdate);
			stmt->bindValue(6, order_type);
			stmt->bindValue(7, fee);

			inserted_recs_count = stmt->execScalar();
		}
		catch (XException &e) {

			Tcerr << _T("Cannot insert a record into table orders using a stmt: ");
			Tcerr << e.what() << std::endl;
		}

		return inserted_recs_count;
	}

	void clearOrdersTable() {

		conn->ExecScalarQuery("DELETE FROM orders");
	}

	std::shared_ptr<const IDbResultSet> getOrdersTableContents() const {

		return conn->ExecQuery("SELECT * FROM orders");
	}

public:
	CMySQLTest() {
	
		connect();
	}

	virtual ~CMySQLTest() { }
};

std::shared_ptr<CMySQLConnection> CMySQLTest::conn;

SUITE(MySQL_tests) {

	TEST_FIXTURE(CMySQLTest, Preparation) {

		dropTable("orders");
		createTableOrders();
	}

	TEST_FIXTURE(CMySQLTest, InsertIntoTable) {

		clearOrdersTable();
		auto recs_count = insertIntoOrders("1", "1", "1991-01-01", "1", \
											"Петров І. І.", "1970-05-06", \
											"КЗ", "1234.56");
		CHECK_EQUAL(1, recs_count);

		recs_count = insertIntoOrders("1", "95", "2016-11-23", "65", \
											"Іванов І. І.", nullptr, \
											"ЗЗП", "0.0");
		CHECK_EQUAL(1, recs_count);

		auto rs = getOrdersTableContents();
		rs->gotoRecord(1);
		bool is_null;
		CDate bdate = rs->getDate(5, is_null);
		CHECK(is_null);
	}

	TEST_FIXTURE(CMySQLTest, InsertIntoTableWithParams) {

		clearOrdersTable();
		auto stmt = getInsertionStmtOrders();

		auto recs_count = insertIntoOrders(stmt, 1, 2, CDate(5, 2, 1992), 1, \
											"Іванов А. А.", CDate(5, 6, 1970), \
											"ЗЗП", "503.7");
		CHECK_EQUAL(1, recs_count);
		recs_count = insertIntoOrders(stmt, 1, 2, CDate(1, 1, 2013), 1, \
											"Іванов А. А.", CDate(), \
											"ЗЗП", "503.7");
		CHECK_EQUAL(1, recs_count);

		auto rs = getOrdersTableContents();
		rs->gotoRecord(1);
		bool is_null;
		CDate bdate = rs->getDate(5, is_null);
		CHECK(is_null);
	}

	TEST_FIXTURE(CMySQLTest, SelectParametrized) {

		clearOrdersTable();
		auto stmt = getInsertionStmtOrders();

		auto recs_count = insertIntoOrders(stmt, 1, 1, CDate(5, 2, 1992), 1, \
											"Іванов А. А.", CDate(5, 6, 1970), \
											"ZZP", "503.7");
		CHECK_EQUAL(1, recs_count);
		recs_count = insertIntoOrders(stmt, 1, 2, CDate(1, 1, 2013), 1, \
											"Іванов А. В.", CDate(), \
											"ЗЗП", "504.7");
		CHECK_EQUAL(1, recs_count);
		recs_count = insertIntoOrders(stmt, 1, 3, CDate(1, 1, 2013), 1, \
											"Іванов В. В.", CDate(), \
											"ЗЗП", "504.7");
		CHECK_EQUAL(1, recs_count);

		auto q_stmt = getConn()->PrepareQuery("SELECT * FROM orders WHERE client_name LIKE ?");
		q_stmt->bindValue(0, "Іванов%");

		auto rs = q_stmt->exec();
		CHECK_EQUAL(3, rs->getRecordsCount());

		q_stmt->bindValue(0, "Іванов А%");
		rs->reload();
		CHECK_EQUAL(2, rs->getRecordsCount());

		q_stmt->bindValue(0, "Іванов В%");
		rs->reload();
		CHECK_EQUAL(1, rs->getRecordsCount());
	}

	TEST_FIXTURE(CMySQLTest, MetadataCheck) {

		auto q_stmt = getConn()->PrepareQuery("SELECT * FROM orders WHERE order_type = 'ZZP'");
		auto rs = q_stmt->exec();

		auto metadata = q_stmt->getResultSetMetadata();
		auto field_nm = metadata->getField(4);
		auto field_type = metadata->getField(6);

		rs->gotoRecord(0);
		auto value_nm = field_nm->getValueAsImmutableString(rs);
		CHECK_EQUAL("Іванов А. А.", value_nm.str);

		auto value_type = field_type->getValueAsImmutableString(rs);
		CHECK_EQUAL("ZZP", value_type.str);
	}

    TEST_FIXTURE(CMySQLTest, LastInsertedId) {

        dropTable("test_dependent");
        dropTable("test_autoinc");
        getConn()->ExecScalarQuery("CREATE TABLE test_autoinc(id INTEGER AUTO_INCREMENT PRIMARY KEY, name CHAR(30) NOT NULL)");
        getConn()->ExecScalarQuery("CREATE TABLE test_dependent(id_dep INTEGER NOT NULL PRIMARY KEY, bdate DATE NOT NULL, FOREIGN KEY(id_dep) REFERENCES test_autoinc(id))");

        auto stmt = getConn()->PrepareQuery("INSERT INTO test_autoinc(name) VALUES(?)");
        stmt->bindValue(0, "John Doe");
        auto stmt2 = getConn()->PrepareQuery("INSERT INTO test_dependent VALUES(?,?)");
        stmt2->bindValue(1, CDate(2, 3, 2020));

        {
            TransactionGuard guard(getConn());

            stmt->execScalar();
            int id = (int) getConn()->getLastInsertedId();
            stmt2->bindValue(0, id);
            stmt2->execScalar();

            guard.commit();
        }

        std::string query = "SELECT t1.id, t1.name, t2.bdate ";
        query += "FROM test_autoinc t1 INNER JOIN test_dependent t2";
        query += " ON t1.id = t2.id_dep";
        auto rs = getConn()->ExecQuery(query.c_str());
        CHECK_EQUAL(1, rs->getRecordsCount());
    }
}