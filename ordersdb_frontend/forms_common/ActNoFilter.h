#pragma once
#include "FilteringComboBox.h"

class CActNoFilter : public CFilteringComboBox {
	void fill() override;

public:
	CActNoFilter(CFilteringManager &filtering_manager_);

	CActNoFilter(const CActNoFilter &obj) = delete;
	CActNoFilter(CActNoFilter &&obj) = default;
	CActNoFilter &operator=(const CActNoFilter &obj) = delete;
	CActNoFilter &operator=(CActNoFilter &&obj) = default;

	virtual ~CActNoFilter();
};

