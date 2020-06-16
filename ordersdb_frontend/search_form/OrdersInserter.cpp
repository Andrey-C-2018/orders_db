#include <basic/XConv.h>
#include <db/DbException.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include <forms_common/ParametersManager.h>
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
	XString<Tchar> center_name;
public:
	ZoneInsertBinder(XComboBox *center_, XWidget *order_date_) : \
				center(center_), order_date(order_date_) {
	
		assert(center);
		assert(order_date);
	}

	ZoneInsertBinder(const Tchar *center_, XWidget *order_date_) : \
				center(nullptr), center_name(center_), \
				order_date(order_date_) {

		assert(!center_name.empty());
		assert(order_date);
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		if (center) {
			center->GetCurrentItemText(center_name);
			if (center_name.empty() || center_name == _T("(0)")) {
				params.error_str += field_name;
				params.error_str += _T(": для визначення зони виберіть Центр у відп. полі");
				params.error_str += _T('\n');
				return true;
			}
		}

		CDate order_date(this->order_date->GetLabel(), CDate::GERMAN_FORMAT);
		if (order_date.isNull()) {
			params.error_str += field_name;
			params.error_str += _T(": для визначення зони необхідна Дата доручення");
			params.error_str += _T('\n');
			return true;
		}

		const Tchar *zone = (center_name != _T("РЦ нБВПД") && order_date >= CDate(1, 1, 2017) ? \
							_T("МЦ") : _T("РЦ"));
		binding_target->bindValue(params.param_no, zone);
		return true;
	}

	virtual ~ZoneInsertBinder() { }
};

//*****************************************************

COrdersInserter::COrdersInserter() : CDbInserter("orders", FIELDS_COUNT), \
						admin_logged(false), \
						center(nullptr), id_order(nullptr), order_date(nullptr), \
						order_type(nullptr), advocat(nullptr), client(nullptr), \
						bdate(nullptr) { }

void COrdersInserter::SetCenterBox(CDbComboBox *center) {

	assert(center);
	assert(!this->center);
	this->center = center;
	center_binder = admin_logged ? \
		std::make_shared<CDbComboBoxInsertBinder>(center, false, false) : nullptr;
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

	const auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();
	assert(id_user != -1);

	int def_center = params_manager.getDefaultCenter();
	assert(0 < def_center && def_center < 10);
	const char center_str[] = {'0' + (char)def_center , 0};

	if(admin_logged) 
		addBinder(0, _T("Центр"), center_binder);
	else
		defStaticInsertion(0, center_str);
	addBinder(1, _T("Номер доручення"), id_order_binder);
	addBinder(2, _T("Дата доручення"), order_date_binder);
	addBinder(3, _T("Тип доручення"), std::make_shared<CDbComboBoxInsertBinder>(order_type, false, false));
	addBinder(4, _T("Адвокат"), std::make_shared<CDbComboBoxInsertBinder>(advocat, false, false));
	addBinder(5, _T("Клієнт"), std::make_shared<UITextInsertBinder>(client, false));
	addBinder(6, _T("Дата народж. клієнта"), std::make_shared<UIDateInsertBinder>(bdate, false));
	defStaticInsertion(7, "0"); // mark
	defStaticInsertion(8, "0"); // KIAS id
	addBinder(9, _T("Користувач"), std::make_shared<CIntInsertBinder>(id_user));
	defStaticInsertion(10, "NOW()");
	defStaticInsertion(11, "NULL"); // cancel reason
	defStaticInsertion(12, "NULL"); // cancel order
	defStaticInsertion(13, "NULL"); // cancel_date
	addBinder(14, _T("Зона відповідальності"), admin_logged ? \
		std::make_shared<ZoneInsertBinder>(center, order_date) : \
		std::make_shared<ZoneInsertBinder>(def_center == 1 ? \
										_T("РЦ нБВПД") : _T("МЦ"), order_date));
		
	CDbInserter::prepare(conn);
}

void COrdersInserter::insert() {

	try {
		CDbInserter::insert();
	}
	catch (CDbException &e) {

		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) return;
		else throw;
	}
}

COrdersInserter::~COrdersInserter() { }
