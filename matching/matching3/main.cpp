#include <iostream>
#include <map>
#include <basic/Exception.h>
#include <basic/PropertiesFile.h>
#include <basic/XConv.h>
#include <basic/locale_init.h>
#include <basic/tstream.h>
#include <csv_file/CsvTable.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>

std::map<Tstring, size_t> fields;

typedef CCsvTable<Tstring> TTable;
typedef IRecord<Tchar> TRecord;

std::shared_ptr<TTable> openCSV(const char *path);
std::map<Tstring, size_t> createFieldNamesMap(std::shared_ptr<TTable> csv);
int convAndCheckIntCell(std::shared_ptr<TRecord> rec, const Tchar *field_name, bool &is_empty);
inline void printOrderDetails(int id_center, int id, \
								const CDate &order_date, const Tstring &act_name);

inline const char *getStringFromResultSet(std::shared_ptr<IDbResultSet> &rs, \
                                            size_t field, char);
inline const Tchar *getStringFromResultSet(std::shared_ptr<IDbResultSet> &rs, \
											size_t field, Tchar);

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
		query += " AND id_stage = ? AND cycle = ? AND act_no = ?";
		auto stmt = mysql_conn->PrepareQuery(query.c_str());

		query = "SELECT id_act_parus, fee_parus, payment_date FROM payments ";
		query += "WHERE id_center = ? AND id_order = ? AND order_date = ?";
		query += " AND id_stage = ? AND cycle = ? AND act_no = ?";
		auto check_stmt = mysql_conn->PrepareQuery(query.c_str());

		size_t rec_count = csv_input->getRecordsCount();
		record_t total_updated_recs = 0;
		Tstring act_name, act_no;
		for (size_t i = 0; i < rec_count; ++i) {
			auto rec = csv_input->getRecord(i);

			act_name = rec->getColValueAsCharArray(fields[_T("act_name")]);
			if (act_name.empty()) continue;
			stmt->bindValue(0, act_name.c_str());

			Tstring fee = rec->getColValueAsCharArray(fields[_T("fee_1C")]);
			auto pos_comma = fee.find(_T(','));
			if (pos_comma != Tstring::npos)
				fee.replace(pos_comma, 1, _T("."));
			stmt->bindValue(1, fee.c_str());

			CDate payment_date(rec->getColValueAsCharArray(fields[_T("payment_date")]), \
								CDate::GERMAN_FORMAT);
			stmt->bindValue(2, payment_date);

			bool is_empty = false;
			int id_center = convAndCheckIntCell(rec, _T("id_center"), is_empty);
			if (is_empty) continue;
			stmt->bindValue(3, id_center);
			check_stmt->bindValue(0, id_center);

			int id = convAndCheckIntCell(rec, _T("id_order"), is_empty);
			if (is_empty) continue;
			stmt->bindValue(4, id);
			check_stmt->bindValue(1, id);

			CDate order_date(rec->getColValueAsCharArray(fields[_T("order_date")]), \
								CDate::GERMAN_FORMAT);
			stmt->bindValue(5, order_date);
			check_stmt->bindValue(2, order_date);

			int id_stage = convAndCheckIntCell(rec, _T("id_stage"), is_empty);
			if (is_empty) continue;
			stmt->bindValue(6, id_stage);
			check_stmt->bindValue(3, id_stage);

			int cycle = convAndCheckIntCell(rec, _T("cycle"), is_empty);
			if (is_empty) continue;
			stmt->bindValue(7, cycle);
			check_stmt->bindValue(4, cycle);

			act_no = rec->getColValueAsCharArray(fields[_T("act_no")]);
			if (act_no.empty()) continue;
			stmt->bindValue(8, act_no.c_str());
			check_stmt->bindValue(5, act_no.c_str());

			auto rs = check_stmt->exec();
			Tchar buffer[CDate::GERMAN_FORMAT_LEN + 1] = _T("");
			auto recs_count = rs->getRecordsCount();
			if (!recs_count) {
				
				Tcerr << _T("Немає такого доручення: рядок ") << i;
				Tcerr << _T(", значення полів: ");
				printOrderDetails(id_center, id, order_date, act_name);
				continue;
			}

			if (payment_date.isNull()) {
                Tcerr << _T("Дата оплати не може бути порожньою: рядок ") << i;
                Tcerr << _T(", значення полів: ");
				printOrderDetails(id_center, id, order_date, act_name);
				continue;
			}

			auto affected_records = stmt->execScalar();
			if(affected_records && recs_count > 0) {
				rs->gotoRecord(0);
				Tcout << _T("Оновлено: ");
                Tcout << id_center << _T('-') << id << _T("-'");
				order_date.toStringGerman(buffer);
                Tcout << buffer << _T("'-") << id_stage << _T('-');
                Tcout << cycle << _T("'-") << act_no << _T(": '");

				const Tchar *act_name_old = getStringFromResultSet(rs, 0, Tchar());
				act_name_old = act_name_old ? act_name_old : _T("NULL");
				Tcout << act_name_old << _T("' <- '") << act_name << _T("', ");

				const Tchar *fee_old = getStringFromResultSet(rs, 1, Tchar());
				fee_old = fee_old ? fee_old : _T("NULL");
				Tcout << fee_old << _T(" <- ") << fee << _T(", '");

				bool is_null = false;
				CDate payment_date_old = rs->getDate(2, is_null);
				payment_date_old.toStringGerman(buffer);
				Tcout << (is_null ? _T("NULL") : buffer);
				payment_date.toStringGerman(buffer);
				Tcout << _T("' <- '") << buffer << _T("'") << std::endl;
			}

			total_updated_recs += affected_records;
		}

		std::wcout << _T("Рядків переглянуто: ") << rec_count;
		std::wcout << _T(", Рядків оновлено: ") << total_updated_recs << std::endl;
	}
	catch (XException &e) {

		std::wcerr << e.what();
		std::wcin.get();
		return 1;
	}
	
	return 0;
}

std::shared_ptr<TTable> openCSV(const char *path) {

	return std::make_shared<TTable>(path, getOS_Locale(), L',', true, false);
}

std::map<Tstring, size_t> createFieldNamesMap(std::shared_ptr<TTable> csv) {
	std::map<Tstring, size_t> fields;
	auto is_found = [&fields](const Tchar *name) {

		auto p = fields.lower_bound(name);
		if (p == fields.end() || fields.key_comp()(p->first, name)) {

			XException e(0, _T("csv-файл не містить поля '"));
			e << name << _T("'");
			throw e;
		}
	};

	size_t fields_count = csv->getColumnsCount();
	for (size_t i = 0; i < fields_count; ++i) {

		auto field_name = csv->getColName(i);
		fields[field_name] = i;
	}

	is_found(_T("id_center"));
	is_found(_T("id_order"));
	is_found(_T("order_date"));
	is_found(_T("id_stage"));
	is_found(_T("cycle"));
	is_found(_T("act_name"));
	is_found(_T("fee_1C"));
	is_found(_T("payment_date"));

	return fields;
}

int convAndCheckIntCell(std::shared_ptr<TRecord> rec, const Tchar *field_name, bool &is_empty) {

	assert(field_name);
	const Tchar *value = rec->getColValueAsCharArray(fields[field_name]);
	if (value && value[0] == _T('\0')) {
		is_empty = true;
		return 0;
	}
	is_empty = false;

	int error = 0;
	int int_value = XConv::ToInt(value, error);

	if (error) {
		XException e(0, _T("Невірне значення: '"));
		e << value << _T("' поля '") << field_name;
		e << _T("'. Це поле може містити тільки числа");
		throw e;
	}

	return int_value;
}

void printOrderDetails(int id_center, int id, \
						const CDate &order_date, const Tstring &act_name) {
	wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];

	Tcerr << _T("id_center = ") << id_center;
    Tcerr << _T(", id_order = ") << id;
	order_date.toStringGerman(buffer);
    Tcerr << _T(", order_date = ") << buffer;
    Tcerr << _T(", act_name = ") << act_name << std::endl;
}

const char *getStringFromResultSet(std::shared_ptr<IDbResultSet> &rs, \
									    size_t field, char) {

    return rs->getString(field);
}

const wchar_t *getStringFromResultSet(std::shared_ptr<IDbResultSet> &rs, \
									    size_t field, wchar_t) {

    return rs->getWString(field);
}