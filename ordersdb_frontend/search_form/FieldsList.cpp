#include "FieldsList.h"
#include <basic/XConv.h>

CFieldsListException::CFieldsListException(const int err_code, const Tchar *err_descr) : \
								XException(err_code, err_descr) { }

CFieldsListException::CFieldsListException(const CFieldsListException &obj) : XException(obj) { }

CFieldsListException::~CFieldsListException() { }

//****************************************************************************

FieldsList::FieldsList() {

	memset(indexes, 0xff, sizeof(indexes));
}

void FieldsList::open(const Tchar *field_indexes_str) {

	assert (indexes[0] == (size_t)-1);
	Tstring field_indexes = field_indexes_str;
	field_indexes += ',';

	size_t unique_checker[FIELDS_COUNT];
	memset(unique_checker, 0xff, sizeof(unique_checker));

	size_t i = 0;
	Tstring::size_type p = field_indexes.find(_T(','));
	Tstring::size_type prev_p = 0;
	while (i < FIELDS_COUNT && p != Tstring::npos) {
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
		p = field_indexes.find(_T(','), prev_p);

		i++;
	}

	if (i == FIELDS_COUNT && p != Tstring::npos)
		throw CFieldsListException(CFieldsListException::E_OUT_OF_RANGE, \
									_T("fields count is out of range"));

	if (i < FIELDS_COUNT)
		throw CFieldsListException(CFieldsListException::E_NOT_ALL_USED, \
									_T("not all fields used"));
}

void FieldsList::open(const IProperties &props) {

	Tstring field_indexes_buffer;
	Tstring field_indexes_str = props.getStringProperty(_T("query_fields"), field_indexes_buffer);
	open(field_indexes_str.c_str());
}

size_t FieldsList::getFieldIndex(Tstring field_indexes_str, Tstring::size_type first, Tstring::size_type last) {

	const size_t MAX_FIELD_DIGITS_COUNT = 2;
	Tchar buffer[MAX_FIELD_DIGITS_COUNT + 1];

	size_t field_digits = last - first;
	if (field_digits == 0 || field_digits > MAX_FIELD_DIGITS_COUNT) {
		CFieldsListException e(CFieldsListException::E_TOO_LARGE, \
									_T("too large index, value: "));
		e << field_indexes_str.substr(first, field_digits);
		throw e;
	}

	Tstrncpy(buffer, &field_indexes_str[first], field_digits);
	buffer[field_digits] = _T('\0');

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

	Tchar field_names[FIELDS_COUNT + 1][21] = {"a.zone", "c.center_short_name", "b.adv_name_short", "a.id", "a.order_date",\
				 "t.type_name", "a.client_name", "a.bdate", "sta.stage_name", "aa.cycle",\
				 "aa.fee", "aa.fee_parus", "aa.outg_post", "aa.outg_daynight", "aa.act_no", "aa.id_act",\
				 "aa.act_reg_date", "aa.act_date", "aa.bank_reg_date", "aa.payment_date",\
				 "aa.article", "uu.user_full_name", "a.reason", "a.cancel_order", "a.cancel_date", "inf.informer_name",\
				 "aa.age", "aa.inv", "aa.lang", "aa.ill", "aa.zek", "aa.vpr", "aa.reduce", "aa.change_", \
				 "aa.close", "aa.zv", "aa.min", "aa.nm_suv", "aa.zv_kr", "aa.No_Ch_Ist", "aa.Koef",
				 "aa.id_stage", "a.id_center_legalaid", "a.id_adv", "a.id_order_type", "aa.id_informer", "aa.id_checker",\
				 "?"};

	Tstring list = field_names[indexes[0]];
	for (int i = 1; i < FIELDS_COUNT; i++) {
		list += _T(',');
		list += field_names[indexes[i]];
	}

	return list;
}
