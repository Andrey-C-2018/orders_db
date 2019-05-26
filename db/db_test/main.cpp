#include <iostream>
#include <db/MySQL/MySQLConnection.h>
//#include <db/SQLite/SQLiteConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>
#include <basic/Exception.h>

int main() {

	setlocale(LC_ALL, "ukr_ukr.1251");
	//setlocale(LC_ALL, "uk_UA.UTF-8");
	{
		CMySQLConnection conn;
	
		try {
			conn.Connect("192.168.1.4", 3310, "root", "12345", "orders");
			//conn.Connect("c:\\#projects\\test", 0, nullptr, nullptr, "test");

			//std::string query = "SELECT t.id_type, a.id_center, a.id, a.order_date, t.type_name, a.client_name ";
			//query += "FROM orders a INNER JOIN order_types t ON a.id_order_type = t.id_type";
			std::string query = "SELECT aa.id_center, aa.id_order, aa.order_date, aa.id_stage, aa.cycle,";
			query += " b.adv_name, aa.act_date, aa.fee as fee_DB, aa.id_act ";
			query += "FROM orders a INNER JOIN payments aa ON";
			query += " a.id_center_legalaid = aa.id_center AND a.id = aa.id_order AND a.order_date = aa.order_date";
			query += " INNER JOIN advocats b ON a.id_adv = b.id_advocat ";
			query += "WHERE aa.act_date >= '2019-01-01' AND aa.is_paid IS NULL AND aa.fee <> 0 ";
			query += "ORDER BY adv_name, id_act";

			auto stmt = conn.PrepareQuery(query.c_str());
			auto m = stmt->getResultSetMetadata();

			size_t fields_count = m->getFieldsCount();
			for (size_t i = 0; i < fields_count; ++i) {
				auto field = m->getField(i);

				std::cout << field->getFieldName() << ' ';
				std::cout << field->getTableName() << ' ';
				std::cout << field->getFieldMaxLength() << ' ';
				std::cout << field->isPrimaryKey() << ' ' << std::endl;
			}
			auto result_set = stmt->exec();
			size_t records_count = result_set->getRecordsCount();
			for (size_t i = 0; i < records_count; ++i) {
				result_set->gotoRecord(i);
				bool is_null;

				int id_center = result_set->getInt(1, is_null);
				int id = result_set->getInt(2, is_null);
				auto client = result_set->getWString(5);
				auto order_date = result_set->getWString(3);

				std::wcout << id_center << ' ' << id << ' ' << client;
				std::wcout << ' ' << order_date << "<--\n";
			}
			result_set->gotoRecord(1);

			auto stmt2 = conn.PrepareQuery("UPDATE orders SET order_date = ? WHERE id = 1");
			stmt2->bindValue(0, CDate(2, 1, 1991));
			stmt2->execScalar();

			//auto stmt2 = conn.PrepareQuery("UPDATE order_types SET type_name = ? WHERE id_type = ?");
			//m->getField(0)->getValueAndBindItTo(result_set, stmt2, 1);
			//stmt2->bindValue(0, "_TEST");
			//stmt2->execScalar();

			//result_set = conn.ExecQuery("SELECT id_type, type_name FROM order_types");
			//fields_count = result_set->getFieldsCount();
			//records_count = result_set->getRecordsCount();
			//for (size_t i = 0; i < records_count; ++i) {
			//	result_set->gotoRecord(i);
			//	std::cout << result_set->getInt(0);
			//	std::cout << result_set->getString(1);
			//	std::cout << std::endl;
			//}
		}
		catch (XException &e) {
			std::wcout << e.what() << std::endl;
		}
	}
	std::cin.get();
	return 0;
}