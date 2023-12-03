#include <db/IDbBindingTarget.h>
#include <db_ext/InsParamNoGuard.h>
#include <db_controls/DbComboBox.h>
#include <forms_common/ParametersManager.h>
#include <forms_common/Constants.h>
#include "CenterBinder.h"

CCenterBinder::CCenterBinder(CDbComboBox* center_widget_, const bool deallocate, \
							bool is_admin_, bool stage_insert_) : \
								CVisualInsertBinder(center_widget_, deallocate), \
								center_widget(center_widget_), \
								is_admin(is_admin_), stage_insert(stage_insert_) {

	const auto& params_manager = CParametersManager::getInstance();
	def_center = params_manager.getDefaultCenter();
}

bool CCenterBinder::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
							Params& params, const Tchar* field_name) {

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

	if (!is_admin && ((def_center == REGIONAL && !stage_insert && \
							def_center != center && center != MEDIATION) || \
					  (def_center != REGIONAL && def_center != center))) {

		params.error_str += field_name;
		params.error_str += _T(": заборонено додавати доручення іншого центру");
		params.error_str += _T('\n');
		return true;
	}

	binding_target->bindValue(params.param_no, center);
	return true;
}

CCenterBinder::~CCenterBinder() { }