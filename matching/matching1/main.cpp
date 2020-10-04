#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <cctype>
#include <basic/PropertiesFile.h>
#include <basic/XConv.h>
#include <basic/TextConv.h>
#include <basic/FileSystem.h>
#include <basic/locale_init.h>
#include <date/Date.h>
#include <db/SQLite/SQLiteConnection.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/DbException.h>
#include "DbfTable.h"
#include "Directory.h"

enum Defaults {
	MAX_DBF_FILE_NAME_LEN = 30, \
	MAX_DBF_FIELD_NAME_LEN = 50
};

constexpr const char FIELD_ID[] = "ND";
constexpr const char FIELD_ADV[] = "POLUT";
constexpr const char FIELD_ACT[] = "NDOC";
constexpr const char FIELD_FEE[] = "S";

constexpr const wchar_t FIELD_ID_w[] = L"ND";
constexpr const wchar_t FIELD_ADV_w[] = L"POLUT";
constexpr const wchar_t FIELD_ACT_w[] = L"NDOC";
constexpr const wchar_t FIELD_FEE_w[] = L"S";

std::map<std::wstring, size_t> createFieldsMap();
std::shared_ptr<IDbConnection> createSQLiteConnection(const CPropertiesFile &props);

bool checkFileExt(std::wstring &file_name);
void checkDbfFileStructure(const CDbfTable &table, const std::map<std::wstring, size_t> &fields, \
							const wchar_t *dbf_file_name);

CDate getPaymentDateFromFileName(const wchar_t *file_name);
const wchar_t *checkIfIsEmptyAndConvert(const char *value, \
										const char *field_name, \
										const size_t record_no, \
										const wchar_t *dbf_file_name, \
										std::vector<wchar_t> &buffer);
void rtrimSpaces(std::vector<wchar_t> &str);
void replaceAdvNameIfNecessary(std::vector<wchar_t> &adv_name, \
								std::shared_ptr<IDbStatement> stmt);

int main() {

	setLocaleToCurrOS_Locale();
	CCOMHelper init_COM;

	try {
		auto fields = createFieldsMap();

		std::wcout << _T("Adding acts to act.db ...\n");
		CPropertiesFile props;
		props.open("config.ini");
		
		Tstring props_buffer, dbf_folder_str;
		dbf_folder_str = props.getStringProperty(_T("dbf_folder"), props_buffer);
		COleDbConnection dbf_conn(dbf_folder_str.c_str());
		CDbfTable dbf_table;
		dbf_conn.getTable(dbf_table);

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
		std::vector<char> buffer;
		std::vector<wchar_t> buffer_w;

		size_t dbf_folder_str_size = dbf_folder_str.size();
		dbf_folder_str += L"\\*.dbf";
		bool not_end = dbf_dir.getFirstFile(dbf_folder_str.c_str());
		while (not_end) {
			std::wcout << _T("Processing ") << dbf_dir.getFileName() << std::endl;

			dbf_folder_str.erase(dbf_folder_str_size + 1, dbf_folder_str.size() - dbf_folder_str_size - 1);
			dbf_folder_str += dbf_dir.getFileName();

			if (!checkFileExt(dbf_folder_str)) {
				std::wcerr << L"Невірний тип файлу: " << dbf_folder_str << std::endl;
				not_end = dbf_dir.getNextFile();
				continue;
			}

			CDate payment_date = getPaymentDateFromFileName(dbf_dir.getFileName());

			if (!checkFileExists(dbf_folder_str.c_str())) {
				std::wcerr << L"Неможливо відкрити: " << dbf_folder_str << std::endl;
				not_end = dbf_dir.getNextFile();
				continue;
			}

			dbf_table.open(dbf_dir.getFileName());
			checkDbfFileStructure(dbf_table, fields, dbf_dir.getFileName());
			if (payment_date.isNull()) {

				std::wcerr << _T("Невірний формат імені файлу: '");
				std::wcerr << dbf_dir.getFileName();
				std::wcerr << _T("', по ньому неможливо визначити дату оплати\n");

				not_end = dbf_dir.getNextFile();
				continue;
			}

			bool not_eot = dbf_table.gotoFirstRecord();
			size_t record_no = 0;
			while (not_eot) {
				bool is_null = false;

				checkIfIsEmptyAndConvert(dbf_table.getCStringCellValue(fields[FIELD_ID_w]), \
					FIELD_ID, record_no, dbf_dir.getFileName(), buffer_w);
				int error, v;
				v = XConv::ToInt(&buffer_w[0], error);
				check_stmt->bindValue(0, v);
				auto rs = check_stmt->exec();
				if (!rs->empty()) {
					XException e(0, _T("Рядок з ID = "));
					e << v << _T(" уже було додано в acts.db");
					throw e;
				}

				ins_stmt->bindValue(0, v);

				checkIfIsEmptyAndConvert(dbf_table.getCStringCellValue(fields[FIELD_ADV_w]), \
					FIELD_ADV, record_no, dbf_dir.getFileName(), buffer_w);
				rtrimSpaces(buffer_w);

				replaceAdvNameIfNecessary(buffer_w, adv_check_stmt);
				ins_stmt->bindValue(1, &buffer_w[0]);

				checkIfIsEmptyAndConvert(dbf_table.getCStringCellValue(fields[FIELD_ACT_w]), \
					FIELD_ACT, record_no, dbf_dir.getFileName(), buffer_w);
				rtrimSpaces(buffer_w);
				ins_stmt->bindValue(2, &buffer_w[0]);

				buffer.resize(12);
				const char *fee = dbf_table.getNumericCellValue(fields[FIELD_FEE_w], &buffer[0], 10);
				checkIfIsEmptyAndConvert(fee, FIELD_FEE, record_no, dbf_dir.getFileName(), buffer_w);
				rtrimSpaces(buffer_w);
				ins_stmt->bindValue(3, &buffer_w[0]);

				ins_stmt->bindValue(4, payment_date);
				ins_stmt->bindValue(5, dbf_dir.getFileName());
				ins_stmt->execScalar();

				not_eot = dbf_table.gotoNextRecord();
				++record_no;
			}

			not_end = dbf_dir.getNextFile();
		}

		std::wcout << _T("Done");
	}
	catch (XException &e) {

		std::wcerr << e.what() << std::endl;
		std::wcin.get();
		exit(1);
	}
	catch (std::exception &e) {

		std::wcerr << e.what() << std::endl;
		std::wcin.get();
		exit(1);
	}

	return 0;
}

std::map<std::wstring, size_t> createFieldsMap() {

	std::map<std::wstring, size_t> fields;
	fields[FIELD_ID_w] = 1;
	fields[FIELD_ADV_w] = 35;
	fields[FIELD_ACT_w] = 41;
	fields[FIELD_FEE_w] = 7;

	return fields;
}

std::shared_ptr<IDbConnection> createSQLiteConnection(const CPropertiesFile &props) {

	std::shared_ptr<IDbConnection> conn = std::make_shared<SQLiteConnection>();

	Tstring buffer;
	const Tchar *curr_prop = nullptr;
	std::string db_file_location;

	curr_prop = props.getStringProperty(_T("sqlite_db_file_location"), buffer);
	const char *p = UCS16_ToUTF8(curr_prop, -1, db_file_location);

	conn->Connect(p, 0, nullptr, nullptr, "acts");
	return conn;
}

bool checkFileExt(std::wstring &file_name) {

	auto p = file_name.rfind(L'.');
	if (p == std::wstring::npos || \
		(p != std::wstring::npos && file_name.size() - p < 4)) return false;

	bool valid = file_name[p] == L'.';
	++p;
	valid = valid && file_name[p] == L'd' || file_name[p] == L'D';
	++p;
	valid = valid && file_name[p] == L'b' || file_name[p] == L'B';
	++p;
	valid = valid && file_name[p] == L'f' || file_name[p] == L'F';

	return valid;
}

void checkDbfFileStructure(const CDbfTable &table, const std::map<std::wstring, size_t> &fields, \
							const wchar_t *dbf_file_name) {
	assert(dbf_file_name);

	size_t dbf_fields_count = table.getFieldsCount();
	for (auto field_item : fields) {

		if (field_item.second >= dbf_fields_count) {
			XException e(0, _T("Немає такого поля: '"));
			e << field_item.first << _T("' у позиції ");
			e << field_item.second << _T(". Перевірте структуру файлу '");
			e << dbf_file_name;
			throw e;
		}

		const wchar_t *dbf_field_name = table.getFieldName(field_item.second);
		size_t dbf_field_name_size = wcsnlen(dbf_field_name, MAX_DBF_FIELD_NAME_LEN);

		if (field_item.first.size() != dbf_field_name_size || \
			(field_item.first.size() == dbf_field_name_size && \
			!std::equal(field_item.first.cbegin(), \
						field_item.first.cend(), \
				dbf_field_name, [](const wchar_t l, const wchar_t r) -> bool {
				
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

CDate getPaymentDateFromFileName(const wchar_t *file_name) {

	assert(file_name);
	wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];
	size_t file_name_len = wcsnlen(file_name, MAX_DBF_FILE_NAME_LEN);
	
	if (file_name[file_name_len] != L'\0') {
		XException e(0, _T("Довжина імені цього файлу: '"));
		e << file_name << _T("' перевищує ліміт у ") << MAX_DBF_FILE_NAME_LEN;
		e << _T(" символів");
		throw e;
	}

#pragma warning(push)
#pragma warning(disable: 4996)
	wcsncpy(buffer, file_name + file_name_len - 12, 2);
	buffer[2] = L'.';
	wcsncpy(buffer + 3, file_name + file_name_len - 10, 2);
	buffer[5] = L'.';
	wcsncpy(buffer + 6, file_name + file_name_len - 8, 4);
	buffer[10] = L'\0';
#pragma warning(pop)

	return CDate(buffer, CDate::GERMAN_FORMAT);
}

const wchar_t *checkIfIsEmptyAndConvert(const char *value, \
										const char *field_name, \
										const size_t record_no, \
										const wchar_t *dbf_file_name, \
										std::vector<wchar_t> &buffer) {

	assert(field_name);
	assert(dbf_file_name);

	if (!value || (value && value[0] == '\0')) {
		XException e(0, _T("Поле '"));
		e << field_name << _T("' = NULL або порожнє, Рядок ");
		e << record_no << _T(" у файлі '") << dbf_file_name << _T("'");
		throw e;
	}

	size_t value_size = strlen(value);
	buffer.resize(value_size + 1);
#pragma warning(push)
#pragma warning(disable: 4996)
	mbstowcs(&buffer[0], value, value_size + 1);
#pragma warning(pop)

	return &buffer[0];
}

void rtrimSpaces(std::vector<wchar_t> &str) {

	size_t i = str.empty() ? 0 : str.size() - 1;
	
	size_t j = i;
	while (i > 0 && (str[i] == L' ' || str[i] == L'\0')) --i;

	if (j > i) {
		++i;
		str[i] = L'\0';
		str.erase(str.begin() + i + 1, str.end());
	}
}

void replaceAdvNameIfNecessary(std::vector<wchar_t> &adv_name, \
								std::shared_ptr<IDbStatement> stmt) {

	stmt->bindValue(0, &adv_name[0]);

	auto result_set = stmt->exec();
	size_t records_count = result_set->getRecordsCount();
	if (!records_count) return;

	result_set->gotoRecord(0);
	auto correct_adv_name = result_set->getImmutableWString(0);
	assert(!correct_adv_name.isNull());
	adv_name.resize(correct_adv_name.size + 1);

#pragma warning(push)
#pragma warning(disable: 4996)
	wcsncpy(&adv_name[0], correct_adv_name.str, correct_adv_name.size);
	adv_name[correct_adv_name.size] = L'\0';
#pragma warning(pop)
}