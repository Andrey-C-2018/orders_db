#include <basic/XConv.h>
#include <db/DbException.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include "OrdersInserter.h"

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

class ZoneInsertBinder : public IInsertBinder {
	XComboBox *center;
	XWidget *order_date;
	XString<Tchar> buffer;
public:
	ZoneInsertBinder(XComboBox *center_, XWidget *order_date_) : \
				center(center_), order_date(order_date_) {
	
		assert(center);
		assert(order_date);
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		center->GetCurrentItemText(buffer);
		if (buffer.empty() || buffer == _T("(0)")) {
			params.error_str += field_name;
			params.error_str += _T(": для визначення зони виберіть Центр у відп. полі");
			params.error_str += _T('\n');
			return true;
		}

		CDate order_date(this->order_date->GetLabel(), CDate::GERMAN_FORMAT);
		if (order_date.isNull()) {
			params.error_str += field_name;
			params.error_str += _T(": для визначення зони необхідна Дата доручення");
			params.error_str += _T('\n');
			return true;
		}

		const char *zone = (buffer == _T("МЦ") && order_date >= CDate(1, 1, 2017) ? \
							"МЦ" : "РЦ");
		binding_target->bindValue(params.param_no, zone);
		return true;
	}

	virtual ~ZoneInsertBinder() { }
};

//*****************************************************

COrdersInserter::COrdersInserter() : CDbInserter("orders", FIELDS_COUNT), \
						center(nullptr), id_order(nullptr), order_date(nullptr), \
						order_type(nullptr), advocat(nullptr), client(nullptr), \
						bdate(nullptr), reason(nullptr), cancel_order(nullptr), \
						cancel_date(nullptr) { }

void COrdersInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(center);
	assert(id_order);
	assert(order_date);
	assert(order_type);
	assert(advocat);
	assert(client);
	assert(bdate);
	assert(reason);
	assert(cancel_order);
	assert(cancel_date);

	int id_user = 1;
	/*auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();*/
	assert(id_user != -1);

	addBinder(0, _T("Центр"), std::make_shared<CDbComboBoxInsertBinder>(center, false));
	addBinder(1, _T("Номер доручення"), std::make_shared<COrderNoBinder>(id_order, false));
	addBinder(2, _T("Дата доручення"), std::make_shared<UIDateInsertBinder>(order_date, false));
	addBinder(3, _T("Тип доручення"), std::make_shared<CDbComboBoxInsertBinder>(order_type, false));
	addBinder(4, _T("Адвокат"), std::make_shared<CDbComboBoxInsertBinder>(advocat, false));
	addBinder(5, _T("Клієнт"), std::make_shared<UITextInsertBinder>(client, false));
	addBinder(6, _T("Дата народж. клієнта"), std::make_shared<UIDateInsertBinder>(bdate, false));
	defStaticInsertion(7, "0"); // mark
	defStaticInsertion(8, "0"); // KIAS id
	defStaticInsertion(9, "NOW()");
	addBinder(10, _T("Користувач"), std::make_shared<CIntInsertBinder>(id_user));
	addBinder(11, _T("Причина скасування"), std::make_shared<UITextInsertBinder>(reason, false));
	addBinder(12, _T("Наказ"), std::make_shared<UITextInsertBinder>(cancel_order, false));
	addBinder(13, _T("Дата скасування"), std::make_shared<UIDateInsertBinder>(cancel_date, false));
	addBinder(14, _T("Зона відповідальності"), std::make_shared<ZoneInsertBinder>(center, order_date));
		
	CDbInserter::prepare(conn);
}

void COrdersInserter::insert() {

	try {
		CDbInserter::insert();
	}
	catch (CDbInserterException &e) {

		ErrorBox(e.what());
	}
	catch (CDbException &e) {

		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			Tstring error_str = _T("Таке доручення уже існує в БД: ");
			error_str += center->GetLabel();
			error_str += _T('-');
			error_str += id_order->GetLabel();
			error_str += _T('-');
			error_str += order_date->GetLabel();
			ErrorBox(error_str.c_str());
		}
		else throw;
	}
}

COrdersInserter::~COrdersInserter() { }
