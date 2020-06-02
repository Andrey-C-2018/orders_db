#include <basic/XConv.h>
#include <db/DbException.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include "OrdersPaymentsInserter.h"

class COrderNoBinder : public CVisualInsertBinder {
public:
	COrderNoBinder(XWidget *order_no_holder, bool free_widget) : \
				CVisualInsertBinder(order_no_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);
		
		size_t size;
		auto order_no_text = widget->GetLabel(size);

		int error;
		int value = XConv::ToInt(order_no_text, error);
		if (error) {
			params.error_str += field_name;
			params.error_str += _T(": введене значення не є числом:");
			params.error_str += order_no_text;
			params.error_str += _T('\n');
			return true;
		}

		if(value < 1 || value > 20000) {
			params.error_str += field_name;
			params.error_str += _T(": невірне значення (має бути від 1 до 20000):");
			params.error_str += order_no_text;
			params.error_str += _T('\n');
			return true;
		}

		binding_target->bindValue(params.param_no, value);
		return true;
	}

	virtual ~COrderNoBinder() { }
};

//*****************************************************

COrdersPaymentsInserter::COrdersPaymentsInserter() : \
					CDbInserter("orders", FIELDS_COUNT) { }

void COrdersPaymentsInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	int id_user = -1;
	/*auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();*/
	assert(id_user != -1);

	addBinder(0, _T("Центр"), std::make_shared<CDbComboBoxInsertBinder>(center));
	addBinder(1, _T("Номер доручення"), std::make_shared<COrderNoBinder>(id_order));
	addBinder(2, _T("Дата доручення"), std::make_shared<UIDateInsertBinder>(order_date));
	addBinder(3, _T("Тип доручення"), std::make_shared<CDbComboBoxInsertBinder>(order_type));
	addBinder(4, _T("Клієнт"), std::make_shared<UITextInsertBinder>(client));
	addBinder(5, _T("Дата народж. клієнта"), std::make_shared<UIDateInsertBinder>(bdate));
	addBinder(6, _T("Причина скасування"), std::make_shared<UITextInsertBinder>(reason));
	addBinder(7, _T("Наказ"), std::make_shared<UITextInsertBinder>(cancel_order));
	addBinder(8, _T("Дата скасування"), std::make_shared<UIDateInsertBinder>(cancel_date));
	addBinder(9, _T("Користувач"), std::make_shared<CIntInsertBinder>(id_user));
	defStaticInsertion(11, "NOW()");
	
	CDbInserter::prepare(conn);
}

void COrdersPaymentsInserter::insert() {

	try {
		CDbInserter::insert();
	}
	catch (CDbInserterException &e) {

		ErrorBox(e.what());
	}
	catch (CDbException &e) {

		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			Tstring error_str = _T("Таке доручення уже існує в БД: ");
			//error_str += stage->GetLabel();
			ErrorBox(error_str.c_str());
		}
		else throw;
	}
}

COrdersPaymentsInserter::~COrdersPaymentsInserter() { }
