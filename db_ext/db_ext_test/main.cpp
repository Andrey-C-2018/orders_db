#include <iostream>
//#include <db/MySQL/MySQLConnection.h>
#include <db/SQLite/SQLiteConnection.h>
#include <db/IDbStatement.h>
#include <db_ext/DbTable.h>
//#include <db_ext/FilteringManager.h>

//class CIntegerBinder : public IBinder {
//	int value;
//public:
//	CIntegerBinder(const int value_) : value(value_) { }
//	void bind(std::shared_ptr<IDbBindingTarget> binding_target, \
//				const size_t param_no) override {
//
//		binding_target->bindValue(param_no, value);
//	}
//	virtual ~CIntegerBinder() { }
//};

int main() {
	//std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();
	std::shared_ptr<IDbConnection> conn = std::make_shared<SQLiteConnection>();

	setlocale(LC_ALL, "ukr_ukr.1251");
	//setlocale(LC_ALL, "uk_UA.UTF-8");
	try {
		//conn->Connect("127.0.0.1", 3306, "orders_stat_user", "123", "orders");
		conn->Connect("c:\\#projects\\test", 0, nullptr, nullptr, "test");

		auto stmt = conn->PrepareQuery("SELECT id_type, type_name, a.* FROM orders a INNER JOIN order_types ON a.id_order_type = order_types.id_type");
		auto rowset = stmt->exec();
		
		rowset->gotoRecord(0);
		std::cout << rowset->getRecordsCount()<< std::endl;

		CQuery query(conn, stmt);
		std::string q_str;
		
		query.getMetaInfo().getUpdateQueryForField(1, q_str);
		std::cout << q_str;
		
		auto upd_stmt = conn->PrepareQuery(q_str.c_str());
		upd_stmt->bindValue(0, "ТЕСТОВИЙ");
		query.getMetaInfo().bindPrimaryKeyValues(1, rowset, upd_stmt);
		
		upd_stmt->execScalar();
		//auto i = 0;
	}
	catch (XException &e) {
		std::cout << e.what() << std::endl;
	}
	/*char expr1[] = "id_center = ?";
	char expr2[] = "id = ? AND order_date > ?";

	CFilteringManager fm;
	auto int_binder = std::make_shared<CIntegerBinder>(10);

	int id1 = fm.addExpr(ImmutableString<char>(expr1, sizeof(expr1)), \
						int_binder);
	int id2 = fm.addExpr(ImmutableString<char>(expr2, sizeof(expr2)), \
						int_binder);
	fm.addBinderToExpr(id2, int_binder);
	fm.enableAll();
	std::cout << fm.getSQLWherePart().str<< std::endl;

	fm.disableExpr(id1);
	std::cout << fm.getSQLWherePart().str;

	std::cin.get();*/
	return 0;
}