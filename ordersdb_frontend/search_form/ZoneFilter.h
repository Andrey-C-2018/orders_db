#pragma once
#include <forms_common/FilteringComboBox.h>

class CZoneFilter : public CFilteringComboBox {
	void fill() override;
public:
	CZoneFilter(CFilteringManager &filtering_manager_);

	CZoneFilter(const CZoneFilter &obj) = delete;
	CZoneFilter(CZoneFilter &&obj) = default;
	CZoneFilter &operator=(const CZoneFilter &obj) = delete;
	CZoneFilter &operator=(CZoneFilter &&obj) = default;

	virtual ~CZoneFilter();
};

