#include <UnitTest++/UnitTest++.h>
#include <basic/PropertiesFile.h>
#include <basic/tstream.h>
#include <ordersdb_frontend/search_form/FieldsList.h>

const Tchar etalon[] = "a.zone,c.center_short_name,b.adv_name_short,a.id,a.order_date,t.type_name,a.client_name,a.bdate,sta.stage_name,aa.cycle,aa.fee,aa.fee_parus,aa.outg_post,aa.outg_daynight,aa.act_no,aa.id_act,aa.act_reg_date,aa.act_date,aa.bank_reg_date,aa.payment_date,aa.article,uu.user_full_name,a.reason,a.cancel_order,a.cancel_date,inf.informer_name,aa.age,aa.inv,aa.lang,aa.ill,aa.zek,aa.vpr,aa.reduce,aa.change_,aa.close,aa.zv,aa.min,aa.nm_suv,aa.zv_kr,aa.No_Ch_Ist,aa.Koef,aa.id_stage,a.id_center_legalaid,a.id_adv,a.id_order_type,aa.id_informer,aa.id_checker";
const Tchar etalon_diff[] = "c.center_short_name,a.zone,b.adv_name_short,a.id,a.order_date,t.type_name,a.client_name,a.bdate,sta.stage_name,aa.cycle,aa.fee,aa.fee_parus,aa.outg_post,aa.outg_daynight,aa.act_no,aa.id_act,aa.act_reg_date,aa.act_date,aa.bank_reg_date,aa.payment_date,aa.article,uu.user_full_name,a.reason,a.cancel_order,a.cancel_date,inf.informer_name,aa.age,aa.inv,aa.lang,aa.ill,aa.zek,aa.vpr,aa.reduce,aa.change_,aa.close,aa.zv,aa.min,aa.nm_suv,aa.zv_kr,aa.No_Ch_Ist,aa.Koef,aa.id_stage,a.id_center_legalaid,a.id_adv,aa.id_informer,a.id_order_type,aa.id_checker";

inline void testCorrectList(const Tchar *list, const Tchar *etalon_, const Tchar *pref);
inline void testWrongList(const Tchar *list, int expected_exception, const Tchar *pref);

SUITE(DirectoryTests) {

	TEST(correct) {
		testCorrectList(_T("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46"), \
						etalon, _T("correct"));
	}

	TEST(correct_diff_order) {
		testCorrectList(_T("1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,45,44,46"), \
						etalon_diff, _T("correct_diff_order"));
	}

	TEST(empty) {
		testWrongList(_T(""), CFieldsListException::E_WRONG_FORMAT, _T("empty"));
	}

	TEST(comma_only) {
		testWrongList(_T(","), CFieldsListException::E_WRONG_FORMAT, _T("comma_only"));
	}

	TEST(space) {
		testWrongList(_T(" "), CFieldsListException::E_NOT_INTEGER, _T("space"));
	}

	TEST(zero_and_letters) {
		testWrongList(_T("0,a"), CFieldsListException::E_NOT_INTEGER, _T("zero_and_letters"));
	}

	TEST(comma_at_begin) {
		testWrongList(_T(",0"), CFieldsListException::E_WRONG_FORMAT, _T("comma_at_begin"));
	}

	TEST(comma_at_end) {
		testWrongList(_T("0,"), CFieldsListException::E_WRONG_FORMAT, _T("comma_at_end"));
	}

	TEST(zero_only) {
		testWrongList(_T("0"), CFieldsListException::E_NOT_ALL_USED, _T("zero_only"));
	}

	TEST(only_two) {
		testWrongList(_T("0,1"), CFieldsListException::E_NOT_ALL_USED, _T("zero_only"));
	}

	TEST(almost_all) {
		testWrongList(_T("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45"), \
		CFieldsListException::E_NOT_ALL_USED, _T("almost_all"));
	}

	TEST(too_many) {
		testWrongList(_T("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47"), \
		CFieldsListException::E_OUT_OF_RANGE, _T("too_many"));
	}

	TEST(zero_duplicate) {
		testWrongList(_T("0,0"), CFieldsListException::E_DUPLICATE, _T("zero_duplicate"));
	}

	TEST(far_duplicate) {
		testWrongList(_T("0,1,2,3,4,5,6,7,8,9,10,11,12,13,2,15"), \
		CFieldsListException::E_DUPLICATE, _T("far_duplicate"));
	}

	TEST(too_large) {
		testWrongList(_T("555"), \
		CFieldsListException::E_TOO_LARGE, _T("too_large"));
	}

	TEST(real) {
		FieldsList fl;

		try {
			CPropertiesFile props;

			props.open("test_data/config.ini");
			fl.open(props);

			Tstring fields_list = fl.getFieldsList();
			CHECK_EQUAL(etalon, fields_list.c_str());
		}
		catch(XException &e) {

			Tcerr << _T("real: ") << e.what() << std::endl;
			CHECK(1 == 0);
		}
	}
}

void testCorrectList(const Tchar *list, const Tchar *etalon_, const Tchar *pref) {

	FieldsList fl;

	try {
		fl.open(list);
		Tstring fields_list = fl.getFieldsList();
		CHECK_EQUAL(etalon_, fields_list.c_str());
	}
	catch(XException &e) {

		Tcerr << pref << _T(": ") << e.what() << std::endl;
		CHECK(1 == 0);
	}
}

void testWrongList(const Tchar *list, int expected_exception, const Tchar *pref) {

	FieldsList fl;

	try {
		fl.open(list);
	}
	catch(CFieldsListException &e) {

		if (expected_exception != e.GetErrorCode())
			Tcout << pref << _T(": ") << e.what() << std::endl;
		CHECK_EQUAL(expected_exception, e.GetErrorCode());
		return;
	}
	catch(XException &e) {

		Tcerr << e.what() << std::endl;
	}
	CHECK(1 == 0);
}
