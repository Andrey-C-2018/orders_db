#include <iostream>
#include <db/DbConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>
#include <basic/Exception.h>

int main() {
	int conn_type = CDbConnectionFactory::DB_CONN_MYSQL;
	auto conn = CDbConnectionFactory::getInstance().createConnection(conn_type);

	setlocale(LC_ALL, "ukr_ukr.1251");
	//setlocale(LC_ALL, "uk_UA.UTF-8");
	try {
		conn->Connect("127.0.0.1", 3306, "orders_stat_user", "123", "orders");

		auto stmt = conn->PrepareQuery("SELECT t.id_type, t.type_name, a.* FROM orders a INNER JOIN order_types t ON a.id_order_type = t.id_type WHERE id = 1212");
		auto m = stmt->getResultSetMetadata();

		size_t fields_count = m->getFieldsCount();
		for (size_t i = 0; i < fields_count; ++i) {
			auto field = m->getField(i);
			
			std::cout << field->getFieldName().str << ' ';
			std::cout << field->getTableName().str << ' ';
			std::cout << field->getFieldMaxLength() << ' ';
			std::cout << field->isPrimaryKey() << ' '<< std::endl;
		}
		auto result_set = stmt->exec();
		size_t records_count = result_set->getRecordsCount();
		for (size_t i = 0; i < records_count; ++i) {
			result_set->gotoRecord(i);
			std::cout << result_set->getInt(0);
			std::cout << result_set->getString(1);
			std::cout << result_set->getInt(2);
			std::cout << std::endl;
		}
		result_set->gotoRecord(3);

		auto stmt2 = conn->PrepareQuery("UPDATE order_types SET type_name = ? WHERE id_type = ?");
		m->getField(0)->getValueAndBindItTo(result_set, stmt2, 1);
		stmt2->bindValue(0, "_TEST");
		stmt2->execScalar();

		result_set = conn->ExecQuery("SELECT id_type, type_name FROM order_types");
		fields_count = result_set->getFieldsCount();
		records_count = result_set->getRecordsCount();
		for (size_t i = 0; i < records_count; ++i) {
			result_set->gotoRecord(i);
			std::cout << result_set->getInt(0);
			std::cout << result_set->getString(1);
			std::cout << std::endl;
		}
	}
	catch (XException &e) {
		std::cout << e.what()<< std::endl;
	}

	std::cin.get();
	return 0;
}