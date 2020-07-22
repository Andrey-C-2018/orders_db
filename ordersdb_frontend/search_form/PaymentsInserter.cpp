#include <basic/XConv.h>
#include <db/DbException.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include <forms_common/FormsInsertionBinders.h>
#include <forms_common/ParametersManager.h>
#include "PaymentsInserter.h"

class CheckerInsertBinder : public IInsertBinder {
	enum {
		NO_CHECKER = 0
	};

	CDbComboBox *checkers_list;
	XWidget *act_reg_date_holder;
	int id_checker;
public:
	CheckerInsertBinder(CDbComboBox *checkers_list_, \
						const int def_center, \
						XWidget *act_reg_date_holder_, \
						std::shared_ptr<IDbConnection> conn) : \
				checkers_list(checkers_list_), \
				act_reg_date_holder(act_reg_date_holder_), id_checker(NO_CHECKER) {
	
		assert(checkers_list);

		auto stmt = conn->PrepareQuery("SELECT MAX(id_user) FROM users WHERE is_def_checker = 1 AND id_center = ?");
		stmt->bindValue(0, def_center);
		auto rs = stmt->exec();

		if (rs->empty()) {
			id_checker = NO_CHECKER;
			return;
		}

		bool is_null;
		rs->gotoRecord(0);
		id_checker = rs->getInt(0, is_null);
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		const Tchar *act_reg_date = act_reg_date_holder->GetLabel();
		if (!act_reg_date || (act_reg_date && act_reg_date[0] == 0)) {
			binding_target->bindValue(params.param_no, NO_CHECKER);
			return true;
		}

		if (checkers_list->isEmptySelection()) 
			binding_target->bindValue(params.param_no, id_checker);
		else
			binding_target->bindValue(params.param_no, \
										checkers_list->getPrimaryKeyAsInteger());

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
						outg_extra(nullptr), outg_post(nullptr), outg_daynight(nullptr), \
						informer(nullptr), id_act(nullptr), \
						act_date(nullptr), act_reg_date(nullptr), payment_date(nullptr), \
						checker(nullptr) { }

void CPaymentsInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(id_order_binder);
	assert(order_date_binder);

	assert(stage);
	assert(cycle);
	assert(article);
	assert(fee);
	assert(outg_extra);
	assert(informer);
	assert(id_act);
	assert(act_date);
	assert(act_reg_date);
	assert(payment_date);
	assert(checker);

	const auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();
	assert(id_user != -1);

	const int def_center = params_manager.getDefaultCenter();
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
	addBinder(16, _T("Різні витрати"), std::make_shared<CFeeBinder>(outg_extra, false, true));
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
	if(def_center == 1)
		addBinder(33, _T("Дата реєстр. в ДКС"), \
					std::make_shared<UIDateInsertBinder>(act_date, false));
	else
		defStaticInsertion(33, "NULL");
	addBinder(34, _T("Перевірив"), \
					std::make_shared<CheckerInsertBinder>(checker, def_center, \
															act_reg_date, conn));
	if(outg_post)
		addBinder(35, _T("Поштові витрати"), \
					std::make_shared<CFeeBinder>(outg_post, false, true));
	else
		defStaticInsertion(35, "0.0");
	if (outg_daynight)
		addBinder(36, _T("Добові витрати"), \
					std::make_shared<CFeeBinder>(outg_daynight, false, true));
	else
		defStaticInsertion(36, "0.0");
	
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