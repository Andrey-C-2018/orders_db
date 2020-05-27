#pragma once
#include "DbComboBox.h"

class CFilteringManager;

class CFilteringDbComboBox : public CDbComboBox {
	CFilteringManager &filtering_manager;
	int id_expr;

	void OnItemChoosed() override;
	void fillComboBox() override;
public:
	CFilteringDbComboBox(CFilteringManager &filtering_manager_, \
						std::shared_ptr<const IDbResultSet> result_set, \
						const size_t field_to_display_, const size_t prim_key_);

	CFilteringDbComboBox(const CFilteringDbComboBox &obj) = delete;
	CFilteringDbComboBox(CFilteringDbComboBox &&obj) = default;
	CFilteringDbComboBox &operator=(const CFilteringDbComboBox &obj) = delete;
	CFilteringDbComboBox &operator=(CFilteringDbComboBox &&obj) = default;

	inline void setExprId(const int id_expr) { this->id_expr = id_expr; }

	virtual ~CFilteringDbComboBox();
};

