#pragma once
#include <editable_grid/EditableCellWidget.h>

class CPostIndexCellWidget : public CEditableCellWidget {
	mutable std::vector<Tchar> label_cache;
	mutable Tstring err_str;
public:
	CPostIndexCellWidget();

	CPostIndexCellWidget(const CPostIndexCellWidget &obj) = delete;
	CPostIndexCellWidget(CPostIndexCellWidget &&obj) = default;
	CPostIndexCellWidget &operator=(const CPostIndexCellWidget &obj) = delete;
	CPostIndexCellWidget &operator=(CPostIndexCellWidget &&obj) = default;

	bool Validate() const override;
	bool Validate(ImmutableString<Tchar> label) const override;

	virtual ~CPostIndexCellWidget();
};

