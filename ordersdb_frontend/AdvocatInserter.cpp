#include <basic/XConv.h>
#include <basic/TextConv.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
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

	auto query = "INSERT INTO advocats VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	stmt = conn->PrepareQuery(query);
}

void CAdvocatInserter::insert() {
	
	size_t param_no = 0;
	Tstring error_str;

	error_str = bindIdAdvocat(param_no);
	error_str += bindAdvName(param_no);
	error_str += bindText(param_no, license_no, _T("Номер свідоцтва"));
	error_str += bindDate(param_no, license_date, _T("Дата свідоцтва"));

	if(!error_str.empty()) ErrorBox(error_str.c_str());
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

	size_t size;
	auto adv_name_str = adv_name->GetLabel(size);
	auto p = Tstrchr(adv_name_str, _T(' '));
	if (!p) return err;
	UCS16_ToUTF8(adv_name_str, (int)(p - adv_name_str), adv_surname);

	while (*p == _T(' ')) ++p;

	auto p2 = Tstrchr(p, _T(' '));
	if (!p2) return err;
	UCS16_ToUTF8(adv_name_str, (int)(p2 - p), adv_name_full);

	while (*p2 == _T(' ')) ++p2;
	if(*p2 == _T('\0')) return err;
	UCS16_ToUTF8(adv_name_str, (int)(adv_name_str + size - p2), adv_patr);

	size_t adv_surname_size = adv_surname.size();
	adv_surname += adv_name_full;
	adv_surname += adv_patr;
	stmt->bindValue(param_no, adv_surname.c_str());

	adv_surname += adv_name_full[0];
	adv_surname += ". ";
	adv_surname += adv_patr[0];
	adv_surname += '.';
	stmt->bindValue(param_no + 1, adv_surname.c_str());

	param_no += 2;
	return Tstring();
}

Tstring CAdvocatInserter::bindText(size_t &param_no, XWidget *text_holder, \
									const Tchar *param_name) {

	auto text = text_holder->GetLabel();
	if(text && text[0] == _T('\0')) {

		Tstring error = param_name;
		error += _T(" не може бути порожнім");
		return error;
	}

	stmt->bindValue(param_no, text); 
	++param_no;
	return Tstring();
}

Tstring CAdvocatInserter::bindDate(size_t &param_no, XWidget *date_holder, \
									const Tchar *param_name) {

	CDate date(date_holder->GetLabel(), CDate::GERMAN_FORMAT);
	if(date.isNull()) {

		Tstring error = param_name;
		error += _T(": невірний формат дати");
		return error;
	}
	if(date > CDate::now()) {

		Tstring error = param_name;
		error += _T(": '");

		Tchar buffer[CDate::GERMAN_FORMAT_LEN + 1];
		date.toStringGerman(buffer);
		error += buffer;

		error += _T("' дата в майбутньому");
		return error;
	}

	stmt->bindValue(param_no, date); 
	++param_no;
	return Tstring();
}

Tstring CAdvocatInserter::bindDbCombo(size_t &param_no, CDbComboBox *db_combo, \
										const Tchar *param_name) {

	auto rs = db_combo->getResultSet();
}

CAdvocatInserter::~CAdvocatInserter() {

	if (adv_name && !adv_name->IsCreated()) delete adv_name;
}
