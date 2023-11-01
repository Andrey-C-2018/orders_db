#include <basic/XConv.h>
#include <db/DbException.h>
#include <db/IDbBindingTarget.h>
#include <db_ext/InsParamNoGuard.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include <forms_common/ParametersManager.h>
#include <forms_common/Constants.h>
#include "OrdersInsertHelper.h"

class CCenterBinder : public CVisualInsertBinder {
	CDbComboBox *center_widget;
	int def_center;
	const bool is_admin;
public:
	CCenterBinder(CDbComboBox *center_widget_, const bool deallocate, \
					const bool is_admin_) : \
				CVisualInsertBinder(center_widget_, deallocate), \
				center_widget(center_widget_), is_admin(is_admin_) {
	
		const auto &params_manager = CParametersManager::getInstance();
		def_center = params_manager.getDefaultCenter();
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		if (center_widget->isEmptySelection()) {
			params.error_str += field_name;
			params.error_str += _T(": не вибраний зі списку");
			params.error_str += _T('\n');
			return true;
		}
		
		int center = center_widget->getPrimaryKeyAsInteger();
		if (def_center != REGIONAL && center == MEDIATION) {
			params.error_str += field_name;
			params.error_str += _T(": медіацію може додавати тільки РЦ");
			params.error_str += _T('\n');
			return true;
		}

		if (!is_admin && ((def_center == REGIONAL && def_center != center && \
								center != MEDIATION) || \
						(def_center != REGIONAL && ((center != REGIONAL && \
								def_center != center) || center == MEDIATION)))) {

			params.error_str += field_name;
			params.error_str += _T(": заборонено додавати доручення іншого центру");
			params.error_str += _T('\n');
			return true;
		}

		binding_target->bindValue(params.param_no, center);
		return true;
	}

	virtual ~CCenterBinder() { }
};

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

class COrderDateBinder : public CVisualInsertBinder {
	CDbComboBox *center_widget;

public:
	COrderDateBinder(XWidget *order_date_holder, CDbComboBox *center_widget_, \
					bool free_widget) : \
				CVisualInsertBinder(order_date_holder, free_widget), \
				center_widget(center_widget_) {
	
		assert(center_widget);
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto date_str = widget->GetLabel(size);
		if (size == 0) {
			params.error_str += field_name;
			params.error_str += _T(": дата не введена\n");
			return true;
		}

		CDate date(date_str, CDate::GERMAN_FORMAT);
		if (date.isNull()) {

			params.error_str += field_name;
			params.error_str += _T(": невірний формат дати\n");
			return true;
		}
		if (date > CDate::now()) {

			params.error_str += field_name;
			params.error_str += _T(": '");
			params.error_str += date_str;

			params.error_str += _T("' дата в майбутньому\n");
			return true;
		}

		if (center_widget->isEmptySelection()) {
			params.error_str += field_name;
			params.error_str += _T(": неможливо перевірити, оскільки Центр не вибраний");
			params.error_str += _T('\n');
			return true;
		}

		int center = center_widget->getPrimaryKeyAsInteger();
		if ((center != REGIONAL && date < CDate(1, 7, 2015)) || \
			(center == REGIONAL && date < CDate(1, 1, 2013))) {

			params.error_str += field_name;
			params.error_str += _T(": '");
			params.error_str += date_str;

			params.error_str += _T("' дата до створення Центру\n");
			return true;
		}

		binding_target->bindValue(params.param_no, date);
		return true;
	}

	virtual ~COrderDateBinder() { }
};

class ZoneInsertBinder : public IInsertBinder {
	CDbComboBox *center_widget;
	XWidget *order_date;
public:
	ZoneInsertBinder(CDbComboBox *center_widget_, XWidget *order_date_) : \
				center_widget(center_widget_), order_date(order_date_) {
	
		assert(center_widget);
		assert(order_date);
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		if (center_widget->isEmptySelection()) {
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

		int center = center_widget->getPrimaryKeyAsInteger();
		const Tchar *zone = (center != REGIONAL && order_date >= CDate(1, 1, 2017) ? \
							_T("МЦ") : _T("РЦ"));
		binding_target->bindValue(params.param_no, zone);
		return true;
	}

	virtual ~ZoneInsertBinder() { }
};

//*****************************************************

COrdersInsertHelper::COrdersInsertHelper() : ins_helper(FIELDS_COUNT), \
						admin_logged(false), \
						center(nullptr), id_order(nullptr), order_date(nullptr), \
						order_type(nullptr), defender(nullptr), client(nullptr), \
						bdate(nullptr) { }

void COrdersInsertHelper::SetCenterBox(CDbComboBox *center) {

	assert(center);
	assert(!this->center);
	this->center = center;
	center_binder = std::make_shared<CCenterBinder>(center, false, admin_logged);
}

void COrdersInsertHelper::SetIdOrderWidget(XWidget *id_order) {

	assert(id_order);
	assert(!this->id_order);
	this->id_order = id_order;
	id_order_binder = std::make_shared<COrderNoBinder>(id_order, false);
}

void COrdersInsertHelper::SetOrderDateWidget(XWidget *order_date) {

	assert(order_date);
	assert(!this->order_date);
	this->order_date = order_date;
	order_date_binder = std::make_shared<COrderDateBinder>(order_date, center, false);
}

void COrdersInsertHelper::createBinders() {

	assert(center);
	assert(id_order);
	assert(order_date);
	assert(order_type);
	assert(defender);
	assert(client);
	assert(bdate);

	const auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();
	assert(id_user != -1);

	ins_helper.addBinder(0, _T("Центр"), center_binder);
	ins_helper.addBinder(1, _T("Номер доручення"), id_order_binder);
	ins_helper.addBinder(2, _T("Дата доручення"), order_date_binder);
	ins_helper.addBinder(3, _T("Тип доручення"), \
			std::make_shared<CDbComboBoxInsertBinder>(order_type, false, false));
	ins_helper.addBinder(4, _T("Адвокат"), \
			std::make_shared<CDbComboBoxInsertBinder>(defender, false, false));
	ins_helper.addBinder(5, _T("Клієнт"), \
						std::make_shared<UITextInsertBinder>(client, false));
	ins_helper.addBinder(6, _T("Дата народж. клієнта"), \
						std::make_shared<UIDateInsertBinder>(bdate, false));
	ins_helper.addBinder(7, _T("Користувач"), \
						std::make_shared<CIntInsertBinder>(id_user));
	ins_helper.addBinder(8, _T("Зона відповідальності"),  \
						std::make_shared<ZoneInsertBinder>(center, order_date));
}

void COrdersInsertHelper::appendOrderInfo(Tstring &err_str) const {

	err_str += _T("№ ");
	err_str += id_order->GetLabel();
	err_str += _T(" від ");
	err_str += order_date->GetLabel();
	err_str += _T(", центр - ");
	err_str += center->GetLabel();
}

void COrdersInsertHelper::resetControls() {

	bdate->SetLabel(_T(""));
}

COrdersInsertHelper::~COrdersInsertHelper() { }
