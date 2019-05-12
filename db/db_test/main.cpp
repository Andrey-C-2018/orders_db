#include <iostream>
//#include <db/MySQL/MySQLConnection.h>
#include <db/SQLite/SQLiteConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>
#include <basic/Exception.h>

int main() {
	SQLiteConnection conn;

	setlocale(LC_ALL, "ukr_ukr.1251");
	//setlocale(LC_ALL, "uk_UA.UTF-8");
	try {
		//conn.Connect("127.0.0.1", 3310, "orders_stat_user", "123", "orders");
		conn.Connect("c:\\#projects\\test", 0, nullptr, nullptr, "test");

		auto stmt = conn.PrepareQuery("SELECT t.id_type, t.type_name, a.* FROM orders a INNER JOIN order_types t ON a.id_order_type = t.id_type WHERE id = 858");
		auto m = stmt->getResultSetMetadata();

		size_t fields_count = m->getFieldsCount();
		for (size_t i = 0; i < fields_count; ++i) {
			auto field = m->getField(i);
			
			std::cout << field->getFieldName() << ' ';
			std::cout << field->getTableName() << ' ';
			std::cout << field->getFieldMaxLength() << ' ';
			std::cout << field->isPrimaryKey() << ' '<< std::endl;
		}
		auto result_set = stmt->exec();
		size_t records_count = result_set->getRecordsCount();
		for (size_t i = 0; i < records_count; ++i) {
			result_set->gotoRecord(i);
			std::wcout << result_set->getInt(0);
			std::wcout << result_set->getWString(1);
			std::wcout << result_set->getInt(2)<< "<--";
			std::wcout << std::endl;
		}
		result_set->gotoRecord(3);

		/*auto stmt2 = conn.PrepareQuery("UPDATE order_types SET type_name = ? WHERE id_type = ?");
		m->getField(0)->getValueAndBindItTo(result_set, stmt2, 1);
		stmt2->bindValue(0, "_TEST");
		stmt2->execScalar();

		result_set = conn.ExecQuery("SELECT id_type, type_name FROM order_types");
		fields_count = result_set->getFieldsCount();
		records_count = result_set->getRecordsCount();
		for (size_t i = 0; i < records_count; ++i) {
			result_set->gotoRecord(i);
			std::cout << result_set->getInt(0);
			std::cout << result_set->getString(1);
			std::cout << std::endl;
		}*/
	}
	catch (XException &e) {
		std::wcout << e.what()<< std::endl;
	}

	std::cin.get();
	return 0;
}