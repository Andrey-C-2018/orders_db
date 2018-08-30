#include <iostream>
#include <db/MySQLConnection.h>
#include <db/IDbResultSetMetadata.h>

int main() {
	CMySQLConnection conn;

	setlocale(LC_ALL, "ukr_ukr.1251");
	try {
		conn.Connect("127.0.0.1", 3310, "orders_stat_user", "123", "orders");

		auto stmt = conn.PrepareQuery("SELECT t.id_type, t.type_name, a.* FROM orders a INNER JOIN order_types t ON a.id_order_type = t.id_type WHERE id = 1");
		auto m = stmt->getResultSetMetadata();

		size_t fields_count = m->getFieldsCount();
		for (size_t i = 0; i < fields_count; ++i) {
			auto field = m->getField(i);

			std::wcout << field.field_name << L' ';
			std::wcout << field.table_name << L' ';
			std::wcout << field.is_primary_key << L' '<< std::endl;
		}

		auto result_set = stmt->exec();
		auto stmt2 = conn.PrepareQuery("UPDATE order_types SET type_name = ? WHERE id_type = ?");
		result_set->getValueAndBindItTo(0, 0, 1, stmt2);
		stmt2->bindValue(0, "TEST");
		stmt2->execScalar();

		result_set = conn.ExecQuery("SELECT id_type, type_name FROM order_types");
		fields_count = result_set->getFieldsCount();
		size_t records_count = result_set->getRecordsCount();
		for (size_t i = 0; i < records_count; ++i) {
			std::wcout << result_set->getInt(0, i);
			std::wcout << result_set->getWString(1, i);
			std::wcout << std::endl;
		}
	}
	catch (XException &e) {
		std::wcout << e.what()<< std::endl;
	}

	std::wcin.get();
	return 0;
}