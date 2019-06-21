#include <memory>
#include <basic/tstream.h>
#include <UnitTest++/UnitTest++.h>
#include <db/MySQL/MySQLConnection.h>

class CMySQLTest {
	static std::shared_ptr<CMySQLConnection> conn;
	std::string query;

protected:
	
	inline void connect() {
	
		if (!conn) {
				
			try {
				conn = std::make_shared<CMySQLConnection>();
				conn->Connect("127.0.0.1", 3306, "root", "12345", "");
			}
			catch (XException &e) {

				Tcerr << e.what() << std::endl;
				exit(1);
			}
		}
	}

	void createDatabase() {

		try {

			conn->ExecScalarQuery("CREATE DATABASE IF NOT EXISTS test_db COLLATE utf8_unicode_ci");
			conn->SetSchema("test_db");
		}
		catch (XException &e) {

			Tcerr << _T("Cannot create database test_db: ") << e.what() << std::endl;
		}
	}

	void dropTable(const char *table_name) {

		query = u8"DROP TABLE IF EXISTS ";
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

		query = u8"CREATE TABLE orders(id_center_legalaid INT(4) NOT NULL,";
		query += u8" id INT(6) NOT NULL, order_date DATE NOT NULL, id_adv INT(6) NOT NULL,";
		query += u8" client_name VARCHAR(255), order_type ENUM('КЗ','ЗЗП','АДМ'), ";
		query += u8" fee DECIMAL(8,2), PRIMARY KEY (id_center_legalaid, id, order_date))";

		try {
			conn->ExecScalarQuery(query.c_str());
		}
		catch (XException &e) {

			Tcerr << _T("Cannot create table orders: ") << e.what() << std::endl;
		}
	}

	record_t insertIntoOrders(const char *id_center, const char *id, \
								const char *order_date, const char *id_adv, \
								const char *client_name, const char *order_type, 
								const char *fee) {

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
		query += "', '";
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
			stmt = conn->PrepareQuery("INSERT INTO orders VALUES (?,?,?,?,?,?,?)");
		}
		catch (XException &e) {

			Tcerr << _T("Error during the orders table insertion stmt preparation: ");
			Tcerr << e.what() << std::endl;
		}

		return stmt;
	}

	record_t insertIntoOrders(std::shared_ptr<IDbStatement> stmt, \
							const int id_center, const int id, \
							const CDate order_date, const int id_adv, \
							const char *client_name, const char *order_type,
							const char *fee) {

		if (!stmt) return 0;

		record_t inserted_recs_count = 0;
		try {
			stmt->bindValue(0, id_center);
			stmt->bindValue(1, id);
			stmt->bindValue(2, order_date);
			stmt->bindValue(3, id_adv);
			stmt->bindValue(4, client_name);
			stmt->bindValue(5, order_type);
			stmt->bindValue(6, fee);

			inserted_recs_count = stmt->execScalar();
		}
		catch (XException &e) {

			Tcerr << _T("Cannot insert a record into table orders using a stmt: ");
			Tcerr << e.what() << std::endl;
		}

		return inserted_recs_count;
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

		createDatabase();

		dropTable("orders");
		createTableOrders();
	}

	TEST_FIXTURE(CMySQLTest, InsertIntoTable) {

		auto recs_count = insertIntoOrders("1", "1", "1991-01-01", "1", \
											u8"Петров І. І.", u8"КЗ", "1234.56");
		CHECK_EQUAL(1, recs_count);
	}

	TEST_FIXTURE(CMySQLTest, InsertIntoTableWithParams) {

		auto stmt = getInsertionStmtOrders();

		auto recs_count = insertIntoOrders(stmt, 1, 2, CDate(5, 2, 1992), 1, \
											u8"Іванов А. А.", u8"ЗЗП", "503.7");
		CHECK_EQUAL(1, recs_count);
	}
}