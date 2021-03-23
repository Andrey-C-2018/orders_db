#include <db/IDbBindingTarget.h>
#include <db_ext/FilteringManager.h>
#include "ActNoFilter.h"

class CActNoBinder : public IBinder {
	CFilteringComboBox *filter;
	XString<Tchar> buffer;

public:
	CActNoBinder(CFilteringComboBox *act_no_filter_) : filter(act_no_filter_) {

		assert(filter);
	}

	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override {

		if (filter->isEmptySelection()) {
			WarningBox(_T("Не вибрано значення поля 'Тип акту'"));
			return 0;
		}

		filter->GetCurrentItemText(buffer);
		binding_target->bindValue(param_no, buffer.c_str());
		return 1;
	}

	size_t affectedParamsCount() const override { return 1; }

	void reset() override { filter->SetSelectionIndex(0); }

	virtual ~CActNoBinder() { }
};

//*****************************************************

CActNoFilter::CActNoFilter(CFilteringManager &filtering_manager_) : \
							CFilteringComboBox(filtering_manager_) {

	auto binder = std::make_shared<CActNoBinder>(this);
	SetIdExpr(filtering_manager_.addExpr("act_no = ?", binder));
}

void CActNoFilter::fill() {

	XComboBox::AddItem(_T("(0)"));
	XComboBox::AddItem(_T("Винагорода"));
	XComboBox::AddItem(_T("Витрати"));
}

CActNoFilter::~CActNoFilter() { }
