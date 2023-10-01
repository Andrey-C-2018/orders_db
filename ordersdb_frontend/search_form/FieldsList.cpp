#include "FieldsList.h"
#include <basic/XConv.h>
#include <basic/TextConv.h>

CFieldsListException::CFieldsListException(const int err_code, const Tchar *err_descr) : \
								XException(err_code, err_descr) { }

CFieldsListException::CFieldsListException(const CFieldsListException &obj) : XException(obj) { }

CFieldsListException::~CFieldsListException() { }

//****************************************************************************

inline void openAndConv(FieldsList *fl, const char *field_indexes_str) {

	assert(fl);
	assert(field_indexes_str);
	fl->open(field_indexes_str);
}

inline void openAndConv(FieldsList *fl, const wchar_t *field_indexes_str_w) {

	assert(fl);
	assert(field_indexes_str_w);
	std::string conv_buffer;
	int len = (int)wcslen(field_indexes_str_w);

	const char *field_indexes_str = UCS16_ToUTF8(field_indexes_str_w, len, conv_buffer);
	fl->open(field_indexes_str);
}

//****************************************************************************

FieldsList::FieldsList() {

	memset(indexes, 0xff, sizeof(indexes));
}

void FieldsList::open(const char *field_indexes_str) {

	assert (indexes[0] == (size_t)-1);
	std::string field_indexes = field_indexes_str;
	field_indexes += ',';

	size_t unique_checker[FIELDS_COUNT];
	memset(unique_checker, 0xff, sizeof(unique_checker));

	size_t i = 0;
	std::string::size_type p = field_indexes.find(',');
	std::string::size_type prev_p = 0;
	while (i < FIELDS_COUNT && p != std::string::npos) {
		if(p <= prev_p) {
			CFieldsListException e(CFieldsListException::E_WRONG_FORMAT, \
									_T("wrong field list format at position "));
			e << p;
			throw e;
		}

		size_t field_index = getFieldIndex(field_indexes, prev_p, p);
		if (unique_checker[field_index] != (size_t)-1) {
			CFieldsListException e(CFieldsListException::E_DUPLICATE, \
									_T("a duplicate in the field list: "));
			e << field_index;
			throw e;
		}
		indexes[i] = field_index;
		unique_checker[field_index] = field_index;

		prev_p = p + 1;
		p = field_indexes.find(',', prev_p);

		i++;
	}

	if (i == FIELDS_COUNT && p != std::string::npos)
		throw CFieldsListException(CFieldsListException::E_OUT_OF_RANGE, \
									_T("fields count is out of range"));

	if (i < FIELDS_COUNT)
		throw CFieldsListException(CFieldsListException::E_NOT_ALL_USED, \
				_T("not all fields used. Maybe the setting config.ini::query_fields is not OK?"));
}

void FieldsList::open(const IProperties &props) {

	Tstring field_indexes_buffer;
	auto field_indexes_str = props.getStringProperty(_T("query_fields"), field_indexes_buffer);

	if (!field_indexes_str)
		throw CFieldsListException(CFieldsListException::E_PROPERTY_NOT_FOUND, \
			_T("the property 'query_fields' is not found in the config.ini"));

	openAndConv(this, field_indexes_str);
}

size_t FieldsList::getFieldIndex(std::string field_indexes_str, size_t first, size_t last) {

	const size_t MAX_FIELD_DIGITS_COUNT = 2;
	char buffer[MAX_FIELD_DIGITS_COUNT + 1];

	size_t field_digits = last - first;
	if (field_digits == 0 || field_digits > MAX_FIELD_DIGITS_COUNT) {
		CFieldsListException e(CFieldsListException::E_TOO_LARGE, \
									_T("too large index, value: "));
		e << field_indexes_str.substr(first, field_digits);
		throw e;
	}

	Tstrncpy(buffer, &field_indexes_str[first], field_digits);
	buffer[field_digits] = '\0';

	int error;
	size_t field_index = XConv::ToUnsigned(buffer, error);
	if (error) {
		CFieldsListException e(CFieldsListException::E_NOT_INTEGER, \
								_T("the field list item is not an integer: "));
		e << field_indexes_str.substr(first, field_digits);
		throw e;
	}

	if(field_index >= FIELDS_COUNT) {
		CFieldsListException e(CFieldsListException::E_OUT_OF_RANGE, \
								_T("the field index is out of range: "));
		e << field_index;
		throw e;
	}

	return field_index;
}

std::string FieldsList::getFieldsList() const {

	const char field_names[FIELDS_COUNT + 1][21] = {"a.zone", "c.center_short_name", "b.adv_name_short", "a.id", "a.order_date",\
				 "t.type_name", "a.client_name", "a.bdate", "sta.stage_name", "aa.cycle",\
				 "aa.fee", "aa.fee_parus", "aa.outg_post", "aa.outg_daynight", "aa.act_no", "aa.id_act",\
				 "aa.act_reg_date", "aa.act_date", "aa.bank_reg_date", "aa.payment_date",\
				 "aa.article", "uu.user_full_name", "a.reason", "a.cancel_order", "a.cancel_date", "inf.informer_name",\
				 "aa.age", "aa.inv", "aa.lang", "aa.ill", "aa.zek", "aa.appeal_softer", "aa.detect_softer", \
				 "aa.reject_appeal", "aa.vpr", "aa.reduce", "aa.change_kval_kr", "aa.reduce_ep", "aa.change_", \
				 "aa.close", "aa.zv", "aa.min", "aa.nm_suv", "aa.zv_kr", "aa.No_Ch_Ist", \
				 "aa.change_med", "aa.cancel_med", "aa.Koef", \
				 "aa.id_stage", "a.id_center_legalaid", "a.id_adv", "a.id_order_type", "aa.id_informer", "aa.id_checker", \
				 "aa.rejection_date", "aa.rejection_reason", "aa.rec_id", "?"};

	std::string list = field_names[indexes[0]];
	for (int i = 1; i < FIELDS_COUNT; i++) {
		list += ',';
		list += field_names[indexes[i]];
	}

	return list;
}
