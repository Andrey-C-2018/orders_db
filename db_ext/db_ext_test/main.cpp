#include <iostream>
#include <db/MySQL/MySQLConnection.h>
#include <db/IDbStatement.h>
#include <db_ext/DbTable.h>

int main() {
	std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();

	setlocale(LC_ALL, "ukr_ukr.1251");
	//setlocale(LC_ALL, "uk_UA.UTF-8");
	try {
		conn->Connect("127.0.0.1", 3306, "orders_stat_user", "123", "orders");

		auto stmt = conn->PrepareQuery("SELECT id_type, type_name, a.* FROM orders a INNER JOIN order_types ON a.id_order_type = order_types.id_type WHERE id = 1212");
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

	std::cin.get();
	return 0;
}