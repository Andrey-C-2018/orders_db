#include <iostream>
#include <map>
#include <basic/Exception.h>
#include <basic/PropertiesFile.h>
#include <basic/XConv.h>
#include <csv_file/CsvTable.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include "mysql_initializer.h"

std::map<std::wstring, size_t> fields;

typedef CCsvTable<std::wstring> TTable;
typedef IRecord<wchar_t> TRecord;

std::shared_ptr<TTable> openCSV(const char *path);
std::map<std::wstring, size_t> createFieldNamesMap(std::shared_ptr<TTable> csv);
int convAndCheckIntCell(std::shared_ptr<TRecord> rec, const wchar_t *field_name, bool &is_empty);

int main() {

	setlocale(LC_ALL, "ukr_ukr.1251");

	try {
		CPropertiesFile props;
		props.open("config.ini");

		auto csv_input = openCSV("result.csv");
		fields = createFieldNamesMap(csv_input);

		auto mysql_conn = createMySQLConnection(props);
		std::string query = "UPDATE payments SET id_act_parus = ?,";
		query += " fee_parus = ?, payment_date = ? ";
		query += "WHERE id_center = ? AND id_order = ? AND order_date = ?";
		query += " AND id_stage = ? AND cycle = ?";
		auto stmt = mysql_conn->PrepareQuery(query.c_str());
		
		size_t rec_count = csv_input->getRecordsCount();
		record_t total_processed_recs = 0;
		std::wstring act_name;
		for (size_t i = 0; i < rec_count; ++i) {
			auto rec = csv_input->getRecord(i);

			act_name = rec->getColValueAsCharArray(fields[L"act_name"]);
			if (act_name.empty()) continue;
			stmt->bindValue(0, act_name.c_str());
			stmt->bindValue(1, rec->getColValueAsCharArray(fields[L"fee_1C"]));

			CDate payment_date(rec->getColValueAsCharArray(fields[L"payment_date"]), \
								CDate::GERMAN_FORMAT);
			stmt->bindValue(2, payment_date);

			bool is_empty = false;
			int id_center = convAndCheckIntCell(rec, L"id_center", is_empty);
			if (is_empty) continue;
			stmt->bindValue(3, id_center);

			int id = convAndCheckIntCell(rec, L"id_order", is_empty);
			if (is_empty) continue;
			stmt->bindValue(4, id);

			CDate order_date(rec->getColValueAsCharArray(fields[L"order_date"]), \
								CDate::SQL_FORMAT);
			stmt->bindValue(5, order_date);

			int id_stage = convAndCheckIntCell(rec, L"id_stage", is_empty);
			if (is_empty) continue;
			stmt->bindValue(6, id_stage);

			int cycle = convAndCheckIntCell(rec, L"cycle", is_empty);
			if (is_empty) continue;
			stmt->bindValue(7, cycle);

			auto affected_records = stmt->execScalar();
			if (affected_records != 1) {
				wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1] = L"";

				std::wcerr << _T("The record ") << i;
				std::wcerr << _T(" was not processed, fields values are: ");
				std::wcerr << _T("id_center = ") << id_center;
				std::wcerr << _T(", id_order = ") << id;
				order_date.ToStringGerman(buffer);
				std::wcerr << _T(", order_date = ") << buffer;
				std::wcerr << _T(", act_name = ") << act_name << std::endl;
			}

			total_processed_recs += affected_records;
		}

		std::wcout << _T("Всього оброблено рядків: ") << total_processed_recs << std::endl;
	}
	catch (XException &e) {

		std::wcerr << e.what();
		std::wcin.get();
		exit(1);
	}
	
	std::wcin.get();
	return 0;
}

std::shared_ptr<TTable> openCSV(const char *path) {

	return std::make_shared<TTable>(path, "ukr_ukr.1251", L',', true);
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