#include <basic/XConv.h>
#include <basic/TextConv.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/DbException.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include "AdvocatInserter.h"

CAdvocatInserter::CAdvocatInserter() : id_advocat(nullptr), \
			adv_name(nullptr), license_no(nullptr), license_date(nullptr), \
			examiner(nullptr), post_index(nullptr), address(nullptr), \
			tel(nullptr), email(nullptr), adv_bdate(nullptr), district(nullptr), \
			org_name(nullptr), org_type(nullptr) { }

void CAdvocatInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	this->conn = conn;

	assert(id_advocat);
	assert(adv_name);
	assert(license_no);
	assert(license_date);
	assert(examiner);
	assert(post_index);
	assert(address);
	assert(tel);
	assert(email);
	assert(adv_bdate);
	assert(district);
	assert(org_name);
	assert(org_type);

	auto query = "INSERT INTO advocats VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	stmt = conn->PrepareQuery(query);
}

void CAdvocatInserter::insert() {
	
	size_t param_no = 0;
	Tstring error_str;

	error_str = bindIdAdvocat(param_no);
	error_str += bindAdvName(param_no);
	stmt->bindValue(param_no++, 0); //mark
	error_str += bindText(param_no, license_no, _T("Номер свідоцтва"));
	error_str += bindDate(param_no, license_date, _T("Дата свідоцтва"));
	error_str += bindDbCombo(param_no, examiner, _T("Екзаменатор"));
	error_str += bindPostIndex(param_no, post_index);
	error_str += bindText(param_no, address, _T("Адреса"));
	error_str += bindText(param_no, tel, _T("Телефон"));
	error_str += bindText(param_no, email, _T("E-mail"));
	error_str += bindDate(param_no, adv_bdate, _T("Дата народження"));
	error_str += bindDbCombo(param_no, district, _T("Основний район роботи"));
	error_str += bindOrg(param_no, org_name, org_type);

	if(!error_str.empty()) 
		ErrorBox(error_str.c_str());
	else {
		try {
			stmt->execScalar();
		}
		catch (CDbException &e) {
			if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
				error_str = _T("Адвокат із таким ID уже доданий: ");
				error_str += id_advocat->GetLabel();
				ErrorBox(error_str.c_str());
			}
			else throw;
		}
	}
}

Tstring CAdvocatInserter::bindIdAdvocat(size_t &param_no) {

	auto id_advocat_str = id_advocat->GetLabel();
	if (id_advocat_str && id_advocat_str[0] == _T('\0')) {

		auto rs = conn->ExecQuery("SELECT MAX(id_advocat) FROM advocats;");
		assert(rs);
		assert(rs->getRecordsCount());
		rs->gotoRecord(0);

		bool is_null;
		stmt->bindValue(param_no, rs->getInt(0, is_null) + 1);
	}
	else {
		int err;
		int id = XConv::ToInt(id_advocat_str, err);

		if (err) {
			Tstring error = _T("Введене значення для ID адвоката не є числом: ");
			error += id_advocat_str;
			error += _T('\n');
			return error;
		}
		stmt->bindValue(param_no, id);
	}

	++param_no;
	return Tstring();
}

Tstring CAdvocatInserter::bindAdvName(size_t &param_no) {

	auto err = _T("Невірний формат імені адвоката, має бути: '{Прізвище} {Імя} {По-батькові}'\n");
	param_no += 2;

	size_t size;
	auto adv_name_str = adv_name->GetLabel(size);
	auto p = Tstrchr(adv_name_str, _T(' '));
	if (!p) return err;
	UCS16_ToUTF8(adv_name_str, (int)(p - adv_name_str), adv_surname);

	while (*p == _T(' ')) ++p;

	auto p2 = Tstrchr(p, _T(' '));
	if (!p2) return err;
	UCS16_ToUTF8(p, (int)(p2 - p), adv_name_full);

	while (*p2 == _T(' ')) ++p2;
	if(*p2 == _T('\0')) return err;
	UCS16_ToUTF8(p2, (int)(adv_name_str + size - p2), adv_patr);

	adv_surname += ' ';
	size_t adv_surname_size = adv_surname.size();
	adv_surname += adv_name_full;
	adv_surname += ' ';
	adv_surname += adv_patr;
	stmt->bindValue(param_no - 2, adv_surname.c_str());

	adv_surname.erase(adv_surname_size, adv_surname.size() - adv_surname_size);
	size_t ln = getUnicodeSymbolLength(adv_name_full[0]);
	for(size_t i = 0; i < ln && i < 4; ++i)
		adv_surname += adv_name_full[i];
	adv_surname += ". ";
	ln = getUnicodeSymbolLength(adv_patr[0]);
	for (size_t i = 0; i < ln && i < 4; ++i)
		adv_surname += adv_patr[i];
	adv_surname += '.';
	stmt->bindValue(param_no - 1, adv_surname.c_str());

	return Tstring();
}

Tstring CAdvocatInserter::bindText(size_t &param_no, XWidget *text_holder, \
									const Tchar *param_name) {

	size_t size;
	auto text = text_holder->GetLabel(size);
	if(text && text[0] == _T('\0')) {

		Tstring error = param_name;
		error += _T(" не може бути порожнім\n");
		++param_no;
		return error;
	}
	
	std::string text_utf8;
	UCS16_ToUTF8(text, (int)size, text_utf8);
	stmt->bindValue(param_no, text_utf8.c_str());

	++param_no;
	return Tstring();
}

Tstring CAdvocatInserter::bindDate(size_t &param_no, XWidget *date_holder, \
									const Tchar *param_name) {

	CDate date(date_holder->GetLabel(), CDate::GERMAN_FORMAT);
	if(date.isNull()) {

		Tstring error = param_name;
		error += _T(": невірний формат дати\n");
		++param_no;
		return error;
	}
	if(date > CDate::now()) {

		Tstring error = param_name;
		error += _T(": '");

		Tchar buffer[CDate::GERMAN_FORMAT_LEN + 1];
		date.toStringGerman(buffer);
		error += buffer;

		error += _T("' дата в майбутньому\n");
		++param_no;
		return error;
	}

	stmt->bindValue(param_no, date); 
	++param_no;
	return Tstring();
}

Tstring CAdvocatInserter::bindDbCombo(size_t &param_no, CDbComboBox *db_combo, \
										const Tchar *param_name) {

	if (db_combo->isEmpty()) {
		Tstring error = param_name;
		error += _T(": значення із списку не вибрано\n");
		++param_no;
		return error;
	}

	stmt->bindValue(param_no, db_combo->getPrimaryKeyAsInteger());
	++param_no;
	return Tstring();
}

Tstring CAdvocatInserter::bindPostIndex(size_t &param_no, XWidget *post_index_holder) {

	size_t size;
	auto post_index_str = post_index_holder->GetLabel(size);

	if (size < 3 || size > 5) {
		++param_no;
		return _T("Кількість цифр у поштовому індексі - від 3 до 5\n");
	}

	stmt->bindValue(param_no, post_index_str);
	++param_no;
	return Tstring();
}

Tstring CAdvocatInserter::bindOrg(size_t &param_no, XWidget *org_name, XWidget *org_type) {

	size_t name_size;
	auto org_name_str = org_name->GetLabel(name_size);
	size_t type_size;
	auto org_type_str = org_type->GetLabel(type_size);
	param_no += 2;

	if (Tstrncmp(org_type_str, _T("ІНД"), type_size)) {
		if (Tstrncmp(org_type_str, _T("АО"), type_size) && \
			Tstrncmp(org_type_str, _T("АБ"), type_size)) {

			Tstring error = _T("Невірне знаяення типу організації: ");
			error += org_type_str;
			error += _T("Має бути: АО, АБ або ІНД\n");
			return error;
		}

		if (org_name_str && org_name_str[0] == _T('\0')) 
			return _T("Назва адвокатського бюро або об'єднання не може бути порожньою\n");
	}
	else {
		org_name_str = _T("");
		name_size = 0;
	}

	std::string utf8_buffer;
	if (name_size) {
		UCS16_ToUTF8(org_name_str, (int)name_size, utf8_buffer);
		stmt->bindValue(param_no - 2, utf8_buffer.c_str());
	}
	else
		stmt->bindNull(param_no - 2);

	if (!type_size) {
		org_type_str = _T("ІНД");
		type_size = 3;
	}
	UCS16_ToUTF8(org_type_str, (int)type_size, utf8_buffer);
	stmt->bindValue(param_no - 1, utf8_buffer.c_str());
	return Tstring();
}

CAdvocatInserter::~CAdvocatInserter() {

	if (adv_name && !adv_name->IsCreated()) delete adv_name;
	if (license_no && !license_no->IsCreated()) delete license_no;
	if (license_date && !license_date->IsCreated()) delete license_date;
	if (examiner && !examiner->IsCreated()) delete examiner;
	if (post_index && !post_index->IsCreated()) delete post_index;
	if (address && !address->IsCreated()) delete address;
	if (tel && !tel->IsCreated()) delete tel;
	if (email && !email->IsCreated()) delete email;
	if (adv_bdate && !adv_bdate->IsCreated()) delete adv_bdate;
	if (district && !district->IsCreated()) delete district;
	if (org_name && !org_name->IsCreated()) delete org_name;
	if (org_type && !org_type->IsCreated()) delete org_type;
}
