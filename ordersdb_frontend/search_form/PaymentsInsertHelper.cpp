#include <basic/XConv.h>
#include <db/DbException.h>
#include <db_ext/InsParamNoGuard.h>
#include <db_ext/DbInsertHelper.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include <forms_common/FormsInsertionBinders.h>
#include <forms_common/ParametersManager.h>
#include "PaymentsInsertHelper.h"

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

CPaymentsInsertHelper::CPaymentsInsertHelper() : incl_order_props(true) {

	init();
}

CPaymentsInsertHelper::CPaymentsInsertHelper(const bool incl_order_props_) : \
				incl_order_props(incl_order_props_), \
				ins_helper(incl_order_props ? FIELDS_COUNT : FIELDS_COUNT - 4) {

	init();
}

void CPaymentsInsertHelper::createBinders(std::shared_ptr<IDbConnection> conn) {

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
	
	if (incl_order_props) {
		ins_helper.addBinder(0, _T("Центр"), center_binder);
		ins_helper.addBinder(1, _T("Номер доручення"), id_order_binder);
		ins_helper.addBinder(2, _T("Дата доручення"), order_date_binder);
		ins_helper.addBinder(10, _T("Користувач"), \
							std::make_shared<CIntInsertBinder>(id_user));
	}
	ins_helper.addBinder(3, _T("Сума"), \
							std::make_shared<CFeeBinder>(fee, false, true));
	ins_helper.addBinder(4, _T("Етап"), \
			std::make_shared<CDbComboBoxInsertBinder>(stage, false, false));
	ins_helper.addBinder(5, _T("Стаття"), \
						std::make_shared<UITextInsertBinder>(article, false));
	ins_helper.addBinder(6, _T("Інформатор"), \
			std::make_shared<CDbComboBoxInsertBinder>(informer, false, true));
	ins_helper.addBinder(7, _T("Акт"), \
							std::make_shared<CActNameBinder>(id_act, false));
	ins_helper.addBinder(8, _T("Дата акта"), \
		std::make_shared<CActDateBinderNoDb>(order_date, act_date, \
											false, act_reg_date));
	ins_helper.addBinder(9, _T("№ розгляду"), \
							std::make_shared<UIIntInsertBinder>(cycle, false));
	ins_helper.addBinder(11, _T("Дата прийняття акта"), \
						std::make_shared<UIDateInsertBinder>(act_reg_date, false));
	ins_helper.addBinder(12, _T("Дата оплати"), \
						std::make_shared<UIDateInsertBinder>(payment_date, false));
	ins_helper.addBinder(13, _T("Різні витрати"), \
						std::make_shared<CFeeBinder>(outg_extra, false, true));
	if(def_center == 1)
		ins_helper.addBinder(14, _T("Дата реєстр. в ДКС"), \
					std::make_shared<UIDateInsertBinder>(act_date, false));
	else
		ins_helper.defStaticInsertion(14, "NULL");

	ins_helper.addBinder(15, _T("Перевірив"), \
					std::make_shared<CheckerInsertBinder>(checker, def_center, \
															act_reg_date, conn));
	if(outg_post)
		ins_helper.addBinder(16, _T("Поштові витрати"), \
					std::make_shared<CFeeBinder>(outg_post, false, true));
	else
		ins_helper.defStaticInsertion(16, "0.0");

	if (outg_daynight)
		ins_helper.addBinder(17, _T("Добові витрати"), \
					std::make_shared<CFeeBinder>(outg_daynight, false, true));
	else
		ins_helper.defStaticInsertion(17, "0.0");
}

void CPaymentsInsertHelper::errMsgOnPrimKeyDuplicate(Tstring &err_str) const {

	err_str = _T("Стадія уже існує: ");
	err_str += stage->GetLabel();
}

void CPaymentsInsertHelper::errMsgOnInvalidRef(Tstring &err_str) const {

	err_str = _T("Стадія не може бути додана, оскільки ");
	err_str += _T("поточний обліковий запис має право додавати стадії лише до існуючих доручень, а ");
	err_str += _T("задане доручення в БД не існує.\nСтадія: ");
	err_str += stage->GetLabel();
}

void CPaymentsInsertHelper::resetControls() {

	article->SetLabel(_T(""));
	fee->SetLabel(_T(""));
	outg_extra->SetLabel(_T("")); 
	outg_post->SetLabel(_T(""));
	outg_daynight->SetLabel(_T(""));
}

CPaymentsInsertHelper::~CPaymentsInsertHelper() { }