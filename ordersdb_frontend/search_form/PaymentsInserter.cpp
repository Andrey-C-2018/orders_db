#include <basic/XConv.h>
#include <db/DbException.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include <forms_common/FormsInsertionBinders.h>
#include <forms_common/ParametersManager.h>
#include "PaymentsInserter.h"

class CheckerInsertBinder : public IInsertBinder {
	XComboBox *center;
	XString<Tchar> buffer;
	std::map<std::wstring, int> def_checkers;
public:
	CheckerInsertBinder(XComboBox *center_holder, \
						std::shared_ptr<IDbConnection> conn, \
						bool free_widget) : center(center_holder) {
	
		assert(center);

		std::string query = "SELECT c.center_short_name, MAX(u.id_user) ";
		query += "FROM users u INNER JOIN centers c ON u.id_center = c.id_center ";
		query += "WHERE is_def_checker = 1 GROUP BY id_center ORDER BY c.id_center";

		auto rs = conn->ExecQuery(query.c_str());
		const size_t count = rs->getRecordsCount();
		bool is_null;
		for (size_t i = 0; i < count; ++i)
			def_checkers.insert(std::pair<std::wstring, int>(rs->getWString(0), \
															rs->getInt(1, is_null)));
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		center->GetCurrentItemText(buffer);
		if (buffer.empty() || buffer == _T("(0)")) {
			params.error_str += field_name;
			params.error_str += _T(": для визначення Підписувача виберіть Центр у відп. полі");
			params.error_str += _T('\n');
			return true;
		}

		auto p = def_checkers.find(buffer.c_str());
		if (p == def_checkers.end()) {
			params.error_str += field_name;
			params.error_str += _T(": невірне значення поля Центр");
			params.error_str += _T('\n');
			return true;
		}

		binding_target->bindValue(params.param_no, p->second);
		return true;
	}

	virtual ~CheckerInsertBinder() { }
};

class CActDateBinderNoDb : public CVisualInsertBinder {

	CActDateValidatorNoDb validator;
public:
	CActDateBinderNoDb(XWidget *order_date_holder, \
						XWidget *act_date_holder, bool free_widget, \
						XWidget *act_reg_date_holder) : \
				CVisualInsertBinder(act_date_holder, free_widget), \
				validator(order_date_holder, act_reg_date_holder) {	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto date_str = widget->GetLabel(size);
		if (size == 0) {
			binding_target->bindNull(params.param_no);
			return true;
		}

		size_t err_str_size = params.error_str.size();
		CDate date(date_str, CDate::GERMAN_FORMAT);

		if (!validator.validate(ImmutableString<Tchar>(date_str, size), \
			date, params.error_str, field_name)) return false;

		if (err_str_size == params.error_str.size())
			binding_target->bindValue(params.param_no, date);

		return true;
	}

	virtual ~CActDateBinderNoDb() { }
};

//*****************************************************

CPaymentsInserter::CPaymentsInserter() : CDbInserter("payments", FIELDS_COUNT), \
						order_date(nullptr), stage(nullptr), \
						cycle(nullptr), article(nullptr), fee(nullptr), \
						outgoings(nullptr), informer(nullptr), id_act(nullptr), \
						act_date(nullptr), act_reg_date(nullptr), payment_date(nullptr) { }

void CPaymentsInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(id_order_binder);
	assert(order_date_binder);

	assert(stage);
	assert(cycle);
	assert(article);
	assert(fee);
	assert(outgoings);
	assert(informer);
	assert(id_act);
	assert(act_date);
	assert(act_reg_date);
	assert(payment_date);

	const auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();
	assert(id_user != -1);

	int def_center = params_manager.getDefaultCenter();
	const char *qa_part = def_center == 1 ? "NULL" : "0";
	const char center_str[] = { '0' + (char)def_center , 0 };

	if (center_binder)
		addBinder(0, _T("Центр"), center_binder);
	else
		defStaticInsertion(0, center_str);
	addBinder(1, _T("Номер доручення"), id_order_binder);
	addBinder(2, _T("Дата доручення"), order_date_binder);
	addBinder(3, _T("Сума"), std::make_shared<CFeeBinder>(fee, false, true));
	addBinder(4, _T("Етап"), std::make_shared<CDbComboBoxInsertBinder>(stage, false, false));
	addBinder(5, _T("Стаття"), std::make_shared<UITextInsertBinder>(article, false));
	addBinder(6, _T("Інформатор"), std::make_shared<CDbComboBoxInsertBinder>(informer, false, true));
	addBinder(7, _T("Акт"), std::make_shared<CActNameBinder>(id_act, false));
	addBinder(8, _T("Дата акта"), std::make_shared<CActDateBinderNoDb>(order_date, act_date, false, act_reg_date));
	addBinder(9, _T("№ розгляду"), std::make_shared<UIIntInsertBinder>(cycle, false));
	addBinder(10, _T("Користувач"), std::make_shared<CIntInsertBinder>(id_user));
	defStaticInsertion(11, "NOW()");
	defStaticInsertion(12, "NULL");
	defStaticInsertion(13, "NULL");
	addBinder(14, _T("Дата прийняття акта"), std::make_shared<UIDateInsertBinder>(act_reg_date, false));
	addBinder(15, _T("Дата оплати"), std::make_shared<UIDateInsertBinder>(payment_date, false));
	addBinder(16, _T("Витрати"), std::make_shared<CFeeBinder>(outgoings, false, true));
	defStaticInsertion(17, "0.0");
	defStaticInsertion(18, "NULL");
	defStaticInsertion(19, "NULL");
	defStaticInsertion(20, "NULL");
	defStaticInsertion(21, "NULL");
	defStaticInsertion(22, "NULL");
	defStaticInsertion(23, qa_part);
	defStaticInsertion(24, qa_part);
	defStaticInsertion(25, qa_part);
	defStaticInsertion(26, qa_part);
	defStaticInsertion(27, qa_part);
	defStaticInsertion(28, qa_part);
	defStaticInsertion(29, qa_part);
	defStaticInsertion(30, qa_part);
	defStaticInsertion(31, qa_part);
	defStaticInsertion(32, "0.0");
	addBinder(33, _T("Дата реєстр. в ДКС"), std::make_shared<UIDateInsertBinder>(act_date, false));
	defStaticInsertion(34, "0"); // id_checker

	CDbInserter::prepare(conn);
}

void CPaymentsInserter::insert() {

	try {
		CDbInserter::insert();
	}
	catch (CDbException &e) {

		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			Tstring error_str = _T("Така стадія уже існує в цьому дорученні: ");
			error_str += stage->GetLabel();
			ErrorBox(error_str.c_str());
		}
		else if (e.GetErrorCode() == CDbException::E_DB_NO_REF_IN_MASTER_TABLE)
			ErrorBox(_T("Відсутні права на додавання нових доручень. Зверніться до адміністратора БД"));
		else throw;
	}
}

CPaymentsInserter::~CPaymentsInserter() { }