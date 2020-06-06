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

//*****************************************************

CPaymentsInserter::CPaymentsInserter() : CDbInserter("payments", FIELDS_COUNT) { }

void CPaymentsInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(center_binder);
	assert(id_order_binder);
	assert(order_date_binder);

	assert(center);
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

	addBinder(0, _T("Центр"), center_binder);
	addBinder(1, _T("Номер доручення"), id_order_binder);
	addBinder(2, _T("Дата доручення"), order_date_binder);
	addBinder(3, _T("Сума"), std::make_shared<CFeeBinder>(fee, false, true));
	addBinder(4, _T("Етап"), std::make_shared<CDbComboBoxInsertBinder>(stage, false, false));
	addBinder(5, _T("Стаття"), std::make_shared<UITextInsertBinder>(article, false));
	addBinder(6, _T("Інформатор"), std::make_shared<CDbComboBoxInsertBinder>(informer, false, true));
	addBinder(7, _T("Акт"), std::make_shared<CActNameBinder>(id_act, false));
	addBinder(8, _T("Дата акта"), std::make_shared<UIDateInsertBinder>(act_date, false));
	addBinder(9, _T("№ розгляду"), std::make_shared<UIIntInsertBinder>(cycle, false));
	addBinder(10, _T("Користувач"), std::make_shared<CIntInsertBinder>(id_user));
	defStaticInsertion(11, "NOW()");
	defStaticInsertion(12, "NULL");
	defStaticInsertion(13, "NULL");
	addBinder(14, _T("Дата прийняття акта"), std::make_shared<UIDateInsertBinder>(act_reg_date, false));
	defStaticInsertion(15, "NULL");
	addBinder(16, _T("Витрати"), std::make_shared<CFeeBinder>(outgoings, false, true));
	defStaticInsertion(17, "0.0");
	defStaticInsertion(18, "0");
	defStaticInsertion(19, "0");
	defStaticInsertion(20, "0");
	defStaticInsertion(21, "0");
	defStaticInsertion(22, "0");
	defStaticInsertion(23, "0");
	defStaticInsertion(24, "0");
	defStaticInsertion(25, "0");
	defStaticInsertion(26, "0");
	defStaticInsertion(27, "0");
	defStaticInsertion(28, "0");
	defStaticInsertion(29, "0");
	defStaticInsertion(30, "0");
	defStaticInsertion(31, "0");
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
		else throw;
	}
}

CPaymentsInserter::~CPaymentsInserter() { }