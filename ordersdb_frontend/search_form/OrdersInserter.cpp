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

		const Tchar *zone = (buffer != _T("РЦ нБВПД") && order_date >= CDate(1, 1, 2017) ? \
							_T("МЦ") : _T("РЦ"));
		binding_target->bindValue(params.param_no, zone);
		return true;
	}

	virtual ~ZoneInsertBinder() { }
};

//*****************************************************

COrdersInserter::COrdersInserter() : CDbInserter("orders", FIELDS_COUNT), \
						center(nullptr), id_order(nullptr), order_date(nullptr), \
						order_type(nullptr), advocat(nullptr), client(nullptr), \
						bdate(nullptr) { }

void COrdersInserter::SetCenterBox(CDbComboBox *center) {

	assert(center);
	assert(!this->center);
	this->center = center;
	center_binder = std::make_shared<CDbComboBoxInsertBinder>(center, false);
}

void COrdersInserter::SetIdOrderWidget(XWidget *id_order) {

	assert(id_order);
	assert(!this->id_order);
	this->id_order = id_order;
	id_order_binder = std::make_shared<COrderNoBinder>(id_order, false);
}

void COrdersInserter::SetOrderDateWidget(XWidget *order_date) {

	assert(order_date);
	assert(!this->order_date);
	this->order_date = order_date;
	order_date_binder = std::make_shared<UIDateInsertBinder>(order_date, false);
}

void COrdersInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(center);
	assert(id_order);
	assert(order_date);
	assert(order_type);
	assert(advocat);
	assert(client);
	assert(bdate);

	int id_user = 1;
	/*auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();*/
	assert(id_user != -1);

	addBinder(0, _T("Центр"), center_binder);
	addBinder(1, _T("Номер доручення"), id_order_binder);
	addBinder(2, _T("Дата доручення"), order_date_binder);
	addBinder(3, _T("Тип доручення"), std::make_shared<CDbComboBoxInsertBinder>(order_type, false));
	addBinder(4, _T("Адвокат"), std::make_shared<CDbComboBoxInsertBinder>(advocat, false));
	addBinder(5, _T("Клієнт"), std::make_shared<UITextInsertBinder>(client, false));
	addBinder(6, _T("Дата народж. клієнта"), std::make_shared<UIDateInsertBinder>(bdate, false));
	defStaticInsertion(7, "0"); // mark
	defStaticInsertion(8, "0"); // KIAS id
	addBinder(9, _T("Користувач"), std::make_shared<CIntInsertBinder>(id_user));
	defStaticInsertion(10, "NOW()");
	defStaticInsertion(11, "NULL"); // cancel reason
	defStaticInsertion(12, "NULL"); // cancel order
	defStaticInsertion(13, "NULL"); // cancel_date
	addBinder(14, _T("Зона відповідальності"), std::make_shared<ZoneInsertBinder>(center, order_date));
		
	CDbInserter::prepare(conn);
}

bool COrdersInserter::insert() {

	bool result = false;
	try {
		result = CDbInserter::insert();
	}
	catch (CDbInserterException &e) {

		ErrorBox(e.what());
		return false;
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
			return false;
		}
		else throw;
	}

	return result;
}

COrdersInserter::~COrdersInserter() { }
