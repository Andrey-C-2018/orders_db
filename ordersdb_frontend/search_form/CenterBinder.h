#pragma once
#include <db_controls/InsertionBinders.h>

class CDbComboBox;

class CCenterBinder : public CVisualInsertBinder {
	CDbComboBox* center_widget;
	int def_center;
	const bool is_admin;
	const bool stage_insert;

public:
	CCenterBinder(CDbComboBox* center_widget_, const bool deallocate, \
					bool is_admin_, bool stage_insert_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params& params, const Tchar* field_name) override;

	virtual ~CCenterBinder();
};
