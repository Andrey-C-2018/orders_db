#include <db/IDbBindingTarget.h>
#include <db_ext/FilteringManager.h>
#include "PaidFilter.h"

class CPaidBinder : public IBinder {
	enum {
		AFFECTED_PARAMS_COUNT = 3
	};

	CFilteringComboBox *paid_filter;
	XString<Tchar> buffer;

public:
	CPaidBinder(CFilteringComboBox *paid_filter_) : paid_filter(paid_filter_) {

		assert(paid_filter);
	}

	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		const size_t param_no) override {

		if (paid_filter->isEmptySelection()) {
			WarningBox(_T("Не вибрано значення поля 'Оплачено'"));
			return 0;
		}

		paid_filter->GetCurrentItemText(buffer);
		bool is_paid = buffer != _T("неопл.");

		binding_target->bindValue(param_no, is_paid);
		if (is_paid)
			binding_target->bindValue(param_no + 1, buffer.c_str());
		binding_target->bindValue(param_no + 2, !is_paid);

		return AFFECTED_PARAMS_COUNT;
	}

	size_t affectedParamsCount() const override { return AFFECTED_PARAMS_COUNT; }

	void reset() override { paid_filter->SetSelectionIndex(0); }

	virtual ~CPaidBinder() { }
};

//*****************************************************

CPaidFilter::CPaidFilter(CFilteringManager &filtering_manager_) : \
						CFilteringComboBox(filtering_manager_) {

	auto binder = std::make_shared<CPaidBinder>(this);
	SetIdExpr(filtering_manager_.addExpr("(? AND is_paid = ?) OR (? AND is_paid IS NULL)", \
				binder));
}

void CPaidFilter::fill() {

	XComboBox::AddItem(_T("(0)"));
	XComboBox::AddItem(_T("РЦ"));
	XComboBox::AddItem(_T("МЦ"));
	XComboBox::AddItem(_T("неопл."));
}

CPaidFilter::~CPaidFilter() { }
