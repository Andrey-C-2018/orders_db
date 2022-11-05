#include <memory>
#include <iostream>
#include <cctype>
#include <basic/PropertiesFile.h>
#include <basic/XConv.h>
#include <basic/FileSystem.h>
#include <basic/Directory.h>
#include <basic/locale_init.h>
#include <basic/tstream.h>
#include <basic/Path.h>
#include <date/Date.h>
#include <db/SQLite/SQLiteConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "DbfTableAdapter.h"
#include "Convert.h"

enum Defaults {
	MAX_DBF_FILE_NAME_LEN = 30, \
	MAX_DBF_FIELD_NAME_LEN = 50
};

constexpr const Tchar FIELD_ID[] = _T("ND");
constexpr const Tchar FIELD_ADV[] = _T("POLUT");
constexpr const Tchar FIELD_ACT[] = _T("NDOC");
constexpr const Tchar FIELD_FEE[] = _T("S");

std::map<Tstring, size_t> createFieldsMap();
std::shared_ptr<IDbConnection> createSQLiteConnection(const CPropertiesFile &props);

bool checkFileExt(const Tstring &file_name);
void checkDbfFileStructure(const CDbfTableAdapter &table, const std::map<Tstring, size_t> &fields, \
							const Tchar *dbf_file_name);

CDate getPaymentDateFromFileName(const Tchar *file_name);
const Tchar *checkIfIsEmptyAndConvert(const char *value, \
										const Tchar *field_name, \
										size_t record_no, \
										const Tchar *dbf_file_name, \
										std::vector<Tchar> &buffer);
void replaceAdvNameIfNecessary(std::vector<Tchar> &adv_name, \
								const std::shared_ptr<IDbStatement> &stmt);

int main() {

	setLocaleToCurrOS_Locale();

	try {
		auto fields = createFieldsMap();

		Tcout << _T("Adding acts to act.db ...") << std::endl;
		CPropertiesFile props;
		props.open("config.ini");

		CDbfTableAdapter dbf_table;
		Tstring props_buffer;
		Tstring dbf_folder_str = props.getStringProperty(_T("dbf_folder"), props_buffer);
		dbf_table.setDbfDir(dbf_folder_str.c_str());

		auto sqlite_conn = createSQLiteConnection(props);
		std::string query;
		if (props.getIntProperty(_T("clear_dbf"), props_buffer)) {
			sqlite_conn->ExecScalarQuery("DELETE FROM payments_1c");
		}
		query = "INSERT INTO payments_1c(id, adv_name, act_name, fee, payment_date, src_file) ";
		query += "VALUES(?, ?, ?, ?, ?, ?)";
		auto ins_stmt = sqlite_conn->PrepareQuery(query.c_str());

		query = "SELECT id FROM payments_1c WHERE id = ?";
		auto check_stmt = sqlite_conn->PrepareQuery(query.c_str());

		query = "SELECT adv_name FROM adv_aliases WHERE adv_name_1c = ?";
		auto adv_check_stmt = sqlite_conn->PrepareQuery(query.c_str());

		XDirectory dbf_dir;
		
		dbf_folder_str += PATH_SLASH;
		dbf_folder_str += _T("*.DBF");

		std::vector<Tchar> buffer;
		bool not_end = dbf_dir.getFirstFile(dbf_folder_str.c_str());
		while (not_end) {
			auto curr_file = dbf_dir.getFileName();
			Tcout << _T("Processing ") << curr_file << std::endl;

			if (!checkFileExt(curr_file)) {
				Tcerr << _T("Невірний тип файлу: ") << curr_file << std::endl;
				not_end = dbf_dir.getNextFile();
				continue;
			}

			CDate payment_date = getPaymentDateFromFileName(curr_file);
			if (payment_date.isNull()) {

				Tcerr << _T("Невірний формат імені файлу: '");
				Tcerr << curr_file;
				Tcerr << _T("', по ньому неможливо визначити дату оплати\n");

				not_end = dbf_dir.getNextFile();
				continue;
			}

			dbf_table.setDbfFileName(curr_file);
			if (!checkFileExists(dbf_table.getDbfFilePath())) {
				Tcerr << _T("Неможливо відкрити: ") << curr_file << std::endl;
				not_end = dbf_dir.getNextFile();
				continue;
			}

			try {
				dbf_table.open();
			}
			catch (XException &e) {
				Tcerr << e.what() << std::endl;
				not_end = dbf_dir.getNextFile();
				continue;
			}

			checkDbfFileStructure(dbf_table, fields, dbf_dir.getFileName());

			bool not_eot = dbf_table.gotoFirstRecord();
			size_t record_no = 0;
			while (not_eot) {
				checkIfIsEmptyAndConvert(dbf_table.getCStringCellValue(fields[FIELD_ID]), \
											FIELD_ID, record_no, dbf_dir.getFileName(), buffer);
				int error, v;
				v = XConv::ToInt(&buffer[0], error);
				check_stmt->bindValue(0, v);
				auto rs = check_stmt->exec();
				if (!rs->empty()) {
					XException e(0, _T("Рядок з ID = "));
					e << v << _T(" уже було додано в acts.db");
					throw e;
				}

				ins_stmt->bindValue(0, v);

				checkIfIsEmptyAndConvert(dbf_table.getCStringCellValue(fields[FIELD_ADV]), \
											FIELD_ADV, record_no, dbf_dir.getFileName(), buffer);

				replaceAdvNameIfNecessary(buffer, adv_check_stmt);
				ins_stmt->bindValue(1, &buffer[0]);

				checkIfIsEmptyAndConvert(dbf_table.getCStringCellValue(fields[FIELD_ACT]), \
											FIELD_ACT, record_no, dbf_dir.getFileName(), buffer);
				ins_stmt->bindValue(2, &buffer[0]);

				const char *fee = dbf_table.getNumericCellValue(fields[FIELD_FEE]);
				checkIfIsEmptyAndConvert(fee, FIELD_FEE, record_no, dbf_dir.getFileName(), buffer);
				ins_stmt->bindValue(3, &buffer[0]);

				ins_stmt->bindValue(4, payment_date);
				ins_stmt->bindValue(5, dbf_dir.getFileName());
				ins_stmt->execScalar();

				not_eot = dbf_table.gotoNextRecord();
				++record_no;
			}

			not_end = dbf_dir.getNextFile();
		}

		Tcout << _T("Done");
	}
	catch (XException &e) {

		Tcerr << e.what() << std::endl;
		Tcin.get();
		return 1;
	}
	catch (std::exception &e) {

		Tcerr << e.what() << std::endl;
		Tcin.get();
		return 1;
	}
}

std::map<Tstring, size_t> createFieldsMap() {

	std::map<Tstring, size_t> fields;
	fields[FIELD_ID] = 1;
	fields[FIELD_ADV] = 35;
	fields[FIELD_ACT] = 41;
	fields[FIELD_FEE] = 7;

	return fields;
}

std::shared_ptr<IDbConnection> createSQLiteConnection(const CPropertiesFile &props) {

	std::shared_ptr<IDbConnection> conn = std::make_shared<SQLiteConnection>();

	Tstring buffer;
	std::vector<char> buffer2;
	const Tchar *curr_prop;

	curr_prop = props.getStringProperty(_T("sqlite_db_file_location"), buffer);
	const char *db_file_location = convertToCharIfNecessary(curr_prop, buffer2);

	conn->Connect(db_file_location, 0, nullptr, nullptr, "acts");
	return conn;
}

bool checkFileExt(const Tstring &file_name) {

	auto p = file_name.rfind(_T('.'));
	if (p == Tstring::npos || file_name.size() - p < 4) return false;

	bool valid = file_name[p] == _T('.');
	++p;
	valid = valid && file_name[p] == _T('d') || file_name[p] == _T('D');
	++p;
	valid = valid && file_name[p] == _T('b') || file_name[p] == _T('B');
	++p;
	valid = valid && file_name[p] == _T('f') || file_name[p] == _T('F');

	return valid;
}

void checkDbfFileStructure(const CDbfTableAdapter &table, const std::map<Tstring, size_t> &fields, \
							const Tchar *dbf_file_name) {
	assert(dbf_file_name);

	size_t dbf_fields_count = table.getFieldsCount();
	for (const auto &field_item : fields) {

		if (field_item.second >= dbf_fields_count) {
			XException e(0, _T("Немає такого поля: '"));
			e << field_item.first << _T("' у позиції ");
			e << field_item.second << _T(". Перевірте структуру файлу '");
			e << dbf_file_name;
			throw e;
		}

		const Tchar *dbf_field_name = table.getFieldName(field_item.second);
		size_t dbf_field_name_size = Tstrnlen(dbf_field_name, MAX_DBF_FIELD_NAME_LEN);

		if (field_item.first.size() != dbf_field_name_size || \
			(field_item.first.size() == dbf_field_name_size && \
			!std::equal(field_item.first.cbegin(), \
						field_item.first.cend(), \
						dbf_field_name, [](const Tchar l, const Tchar r) -> bool {

			return std::toupper(l) == std::toupper(r);
		}))) {

			XException e(0, _T("Поле у позиції "));
			e << field_item.second << _T(" не є '") << field_item.first;
			e << _T("', як це має бути. Його ім'я - '") << dbf_field_name;
			e << _T("' у файлі '") << dbf_file_name << _T("'");
			throw e;
		}
	}
}

CDate getPaymentDateFromFileName(const Tchar *file_name) {

	assert(file_name);
	Tchar buffer[CDate::GERMAN_FORMAT_LEN + 1];
	size_t file_name_len = Tstrnlen(file_name, MAX_DBF_FILE_NAME_LEN);

	if (file_name[file_name_len] != _T('\0')) {
		XException e(0, _T("Довжина імені цього файлу: '"));
		e << file_name << _T("' перевищує ліміт у ") << MAX_DBF_FILE_NAME_LEN;
		e << _T(" символів");
		throw e;
	}

	Tstrncpy(buffer, file_name + file_name_len - 12, 2);
	buffer[2] = _T('.');
	Tstrncpy(buffer + 3, file_name + file_name_len - 10, 2);
	buffer[5] = _T('.');
	Tstrncpy(buffer + 6, file_name + file_name_len - 8, 4);
	buffer[10] = _T('\0');

	return CDate(buffer, CDate::GERMAN_FORMAT);
}

const Tchar *checkIfIsEmptyAndConvert(const char *value, \
										const Tchar *field_name, \
										size_t record_no, \
										const Tchar *dbf_file_name, \
										std::vector<Tchar> &buffer) {

	assert(field_name);
	assert(dbf_file_name);

	if (!value || value[0] == '\0') {
		XException e(0, _T("Поле '"));
		e << field_name << _T("' = NULL або порожнє, Рядок ");
		e << record_no << _T(" у файлі '") << dbf_file_name << _T("'");
		throw e;
	}

	convertToUTF(value, buffer);
	return &buffer[0];
}

inline ImmutableString<char> getStringFromResultSet(std::shared_ptr<IDbResultSet> &rs, \
													size_t field, char) {

	return rs->getImmutableString(field);
}

inline ImmutableString<wchar_t> getStringFromResultSet(std::shared_ptr<IDbResultSet> &rs, \
														size_t field, wchar_t) {

	return rs->getImmutableWString(field);
}

void replaceAdvNameIfNecessary(std::vector<Tchar> &adv_name, \
								const std::shared_ptr<IDbStatement> &stmt) {

	stmt->bindValue(0, &adv_name[0]);

	auto result_set = stmt->exec();
	size_t records_count = result_set->getRecordsCount();
	if (!records_count) return;

	result_set->gotoRecord(0);
	auto correct_adv_name = getStringFromResultSet(result_set, 0, Tchar());
	
	copyImmutableStrToVector(correct_adv_name, adv_name);
}
