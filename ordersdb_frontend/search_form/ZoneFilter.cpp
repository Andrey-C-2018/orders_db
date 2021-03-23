#include <db/IDbBindingTarget.h>
#include <db_ext/FilteringManager.h>
#include "ZoneFilter.h"

class CZoneBinder : public IBinder {
	CFilteringComboBox *zone_filter;
	XString<Tchar> buffer;

public:
	CZoneBinder(CFilteringComboBox *zone_filter_) : zone_filter(zone_filter_) {

		assert(zone_filter);
	}

	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		const size_t param_no) override {

		if (zone_filter->isEmptySelection()) {
			WarningBox(_T("Не вибрано значення поля 'Зона відповідальності'"));
			return 0;
		}

		zone_filter->GetCurrentItemText(buffer);
		binding_target->bindValue(param_no, buffer.c_str());
		return 1;
	}

	size_t affectedParamsCount() const override { return 1; }

	void reset() override { zone_filter->SetSelectionIndex(0); }

	virtual ~CZoneBinder() { }
};

//*****************************************************

CZoneFilter::CZoneFilter(CFilteringManager &filtering_manager_) : \
							CFilteringComboBox(filtering_manager_) {

	auto binder = std::make_shared<CZoneBinder>(this);
	SetIdExpr(filtering_manager_.addExpr("zone = ?", binder));
}

void CZoneFilter::fill() {

	XComboBox::AddItem(_T("(0)"));
	XComboBox::AddItem(_T("РЦ"));
	XComboBox::AddItem(_T("МЦ"));
}

CZoneFilter::~CZoneFilter() { }
