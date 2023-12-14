#pragma once
#include <db_ext/FilteringManager.h>
#include <db_ext/QueryModifier.h>
#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>
#include <xwindows_ex/XTabStopPanel.h>
#include <forms_common/FlexiblePropepties.h>

struct IDbConnection;
class CDbTable;
class CDbGrid;
class CFilteringEdit;
class CDbComboBoxCellWidget;
class CComboBoxCellWidget;

class InformersBook : public XTabStopPanel {
	enum Defaults {
		DEF_GUI_ROW_HEIGHT = 30, \
		DEF_GUI_VERT_GAP = 5, \
		DEF_GUI_HORZ_GAP = 5
	};

	CFlexiblePropepties props;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<CDbTable> db_table;

	CQueryModifier query_modifier;
	CFilteringManager filtering_manager;
	CFilteringEdit *flt_id, *flt_informer;
	
	CDbGrid *grid;
	CDbComboBoxCellWidget *districts_list;
	CComboBoxCellWidget *org_types_list;

	int grid_x, grid_y;
	int grid_margin_x, grid_margin_y;

	XButton *btn_apply_filter, *btn_add, *btn_remove;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);
	void setFieldsSizes();
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);
	void DisplayWidgets();
	void adjustUIDependentCellWidgets(CDbGrid* grid);
	void initFilteringControls();

	void OnFilterButtonClick(XCommandEvent *eve);
	void OnAddRecordButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);

public:
	InformersBook(XWindow *parent, const int flags, \
					const Tchar *label, \
					const int x, const int y, \
					const int width, const int height);
	
	void OnSize(XSizeEvent *eve);

	virtual ~InformersBook();
};
