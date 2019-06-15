#include <iostream>
#include <map>
#include <basic/Exception.h>
#include <basic/PropertiesFile.h>
#include <basic/XConv.h>
#include <basic/locale_init.h>
#include <csv_file/CsvTable.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>

std::map<std::wstring, size_t> fields;

typedef CCsvTable<std::wstring> TTable;
typedef IRecord<wchar_t> TRecord;

std::shared_ptr<TTable> openCSV(const char *path);
std::map<std::wstring, size_t> createFieldNamesMap(std::shared_ptr<TTable> csv);
int convAndCheckIntCell(std::shared_ptr<TRecord> rec, const wchar_t *field_name, bool &is_empty);

int main() {

	setLocaleToCurrOS_Locale();

	try {
		CPropertiesFile props;
		props.open("config.ini");

		auto csv_input = openCSV("result.csv");
		fields = createFieldNamesMap(csv_input);

		auto mysql_conn = CMySQLConnectionFactory::createConnection(props);
		std::string query = "UPDATE payments SET id_act_parus = ?,";
		query += " fee_parus = ?, payment_date = ? ";
		query += "WHERE id_center = ? AND id_order = ? AND order_date = ?";
		query += " AND id_stage = ? AND cycle = ?";
		auto stmt = mysql_conn->PrepareQuery(query.c_str());

		query = "SELECT id_act_parus, fee_parus, payment_date FROM payments ";
		query += "WHERE id_center = ? AND id_order = ? AND order_date = ?";
		query += " AND id_stage = ? AND cycle = ?";
		auto check_stmt = mysql_conn->PrepareQuery(query.c_str());

		size_t rec_count = csv_input->getRecordsCount();
		record_t total_updated_recs = 0;
		std::wstring act_name;
		for (size_t i = 0; i < rec_count; ++i) {
			auto rec = csv_input->getRecord(i);

			act_name = rec->getColValueAsCharArray(fields[L"act_name"]);
			if (act_name.empty()) continue;
			stmt->bindValue(0, act_name.c_str());

			std::wstring fee = rec->getColValueAsCharArray(fields[L"fee_1C"]);
			auto pos_comma = fee.find(L',');
			if (pos_comma != std::wstring::npos)
				fee.replace(pos_comma, 1, L".");
			stmt->bindValue(1, fee.c_str());

			CDate payment_date(rec->getColValueAsCharArray(fields[L"payment_date"]), \
								CDate::GERMAN_FORMAT);
			stmt->bindValue(2, payment_date);

			bool is_empty = false;
			int id_center = convAndCheckIntCell(rec, L"id_center", is_empty);
			if (is_empty) continue;
			stmt->bindValue(3, id_center);
			check_stmt->bindValue(0, id_center);

			int id = convAndCheckIntCell(rec, L"id_order", is_empty);
			if (is_empty) continue;
			stmt->bindValue(4, id);
			check_stmt->bindValue(1, id);

			CDate order_date(rec->getColValueAsCharArray(fields[L"order_date"]), \
								CDate::GERMAN_FORMAT);
			stmt->bindValue(5, order_date);
			check_stmt->bindValue(2, order_date);

			int id_stage = convAndCheckIntCell(rec, L"id_stage", is_empty);
			if (is_empty) continue;
			stmt->bindValue(6, id_stage);
			check_stmt->bindValue(3, id_stage);

			int cycle = convAndCheckIntCell(rec, L"cycle", is_empty);
			if (is_empty) continue;
			stmt->bindValue(7, cycle);
			check_stmt->bindValue(4, cycle);

			auto rs = check_stmt->exec();
			wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1] = L"";
			auto recs_count = rs->getRecordsCount();
			if (!recs_count) {
				
				std::wcerr << _T("Немає такого доручення: рядок ") << i;
				std::wcerr << _T(", значення полів: ");
				std::wcerr << _T("id_center = ") << id_center;
				std::wcerr << _T(", id_order = ") << id;
				order_date.ToStringGerman(buffer);
				std::wcerr << _T(", order_date = ") << buffer;
				std::wcerr << _T(", act_name = ") << act_name << std::endl;

				continue;
			}

			auto affected_records = stmt->execScalar();
			if(affected_records && recs_count) {
				rs->gotoRecord(0);
				std::wcout << _T("Оновлено: ");
				std::wcout << id_center << _T('-') << id << _T("-'");
				order_date.ToStringGerman(buffer);
				std::wcout << buffer << _T("'-") << id_stage << _T('-');
				std::wcout << cycle << _T(": '");

				const Tchar *act_name_old = rs->getWString(0);
				act_name_old = act_name_old ? act_name_old : _T("NULL");
				std::wcout << act_name_old << _T("' <- '") << act_name << _T("', ");

				const Tchar *fee_old = rs->getWString(1);
				fee_old = fee_old ? fee_old : _T("NULL");
				std::wcout << fee_old << _T(" <- ") << fee << _T(", '");

				bool is_null = false;
				CDate payment_date_old = rs->getDate(2, is_null);
				payment_date_old.ToStringGerman(buffer);
				std::wcout << (is_null ? _T("NULL") : buffer);
				payment_date.ToStringGerman(buffer);
				std::wcout << _T("' <- '") << buffer << _T("'") << std::endl;
			}

			total_updated_recs += affected_records;
		}

		std::wcout << _T("Рядків переглянуто: ") << rec_count;
		std::wcout << _T(", Рядків оновлено: ") << total_updated_recs << std::endl;
	}
	catch (XException &e) {

		std::wcerr << e.what();
		std::wcin.get();
		exit(1);
	}
	
	return 0;
}

std::shared_ptr<TTable> openCSV(const char *path) {

	return std::make_shared<TTable>(path, getOS_Locale(), L',', true, true);
}

std::map<std::wstring, size_t> createFieldNamesMap(std::shared_ptr<TTable> csv) {
	std::map<std::wstring, size_t> fields;
	auto is_found = [&fields](const wchar_t *name) {

		auto p = fields.lower_bound(name);
		if (p == fields.end() || fields.key_comp()(p->first, name)) {

			XException e(0, _T("the input csv does not contain '"));
			e << name << _T("' field");
			throw e;
		}
	};

	size_t fields_count = csv->getColumnsCount();
	for (size_t i = 0; i < fields_count; ++i) {

		auto field_name = csv->getColName(i);
		fields[field_name] = i;
	}

	is_found(L"id_center");
	is_found(L"id_order");
	is_found(L"order_date");
	is_found(L"id_stage");
	is_found(L"cycle");
	is_found(L"act_name");
	is_found(L"fee_1C");
	is_found(L"payment_date");

	return fields;
}

int convAndCheckIntCell(std::shared_ptr<TRecord> rec, const wchar_t *field_name, bool &is_empty) {

	assert(field_name);
	const wchar_t *value = rec->getColValueAsCharArray(fields[field_name]);
	if (value && value[0] == L'\0') {
		is_empty = true;
		return 0;
	}
	is_empty = false;

	int error = 0;
	int int_value = XConv::ToInt(value, error);

	if (error) {
		int a = 0;
		XException e(0, _T("Wrong field value: '"));
		e << value << _T("' of field '") << field_name;
		e << _T("'. The field must contain numbers only");
		throw e;
	}

	return int_value;
}