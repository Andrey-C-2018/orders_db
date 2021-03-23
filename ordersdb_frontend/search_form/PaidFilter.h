#pragma once
#include <forms_common/FilteringComboBox.h>

class CPaidFilter : public CFilteringComboBox {
	void fill() override;

public:
	CPaidFilter(CFilteringManager &filtering_manager_);

	CPaidFilter(const CPaidFilter &obj) = delete;
	CPaidFilter(CPaidFilter &&obj) = default;
	CPaidFilter &operator=(const CPaidFilter &obj) = delete;
	CPaidFilter &operator=(CPaidFilter &&obj) = default;

	virtual ~CPaidFilter();
};
