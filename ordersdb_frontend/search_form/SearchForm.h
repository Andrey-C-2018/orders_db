#pragma once
#include <basic/PropertiesFile.h>
#include <db_ext/FilteringManager.h>
#include <db_ext/QueryModifier.h>
#include <xwindows_ex/XTabStopPanel.h>
#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>

struct IDbConnection;
class CDbTable;
class CDbGrid;
struct CPaymentsConstraints;
class CComboBoxCellWidget;
class CDbComboBoxCellWidget;
class CFilteringEdit;
class CFilteringDbComboBox;

class CSearchForm : public XTabStopPanel {
	enum Defaults {
		DEF_GUI_ROW_HEIGHT = 30, \
		DEF_GUI_VERT_GAP = 5, \
		DEF_GUI_HORZ_GAP = 5
	};

	CPropertiesFile props;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<CDbTable> db_table;

	CQueryModifier query_modifier;
	CFilteringManager filtering_manager;
	CFilteringEdit *flt_id;
	CFilteringDbComboBox *flt_advocat, *flt_center;

	CDbGrid *grid;
	CDbComboBoxCellWidget *advocats_list, *centers_list;
	int grid_x, grid_y;
	int grid_margin_x, grid_margin_y;

	XButton *btn_apply_filter, *btn_add, *btn_remove;
	//CPaymentsInserter inserter;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);
	void setFieldsSizes();
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);
	void displayWidgets();
	void adjustUIDependentCellWidgets(CDbGrid *grid);
	void initFilteringControls();

	void OnFilterButtonClick(XCommandEvent *eve);
	void OnAddRecordButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);
	
public:
	CSearchForm(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	void OnSize(XSizeEvent *eve);

	virtual ~CSearchForm();
};

//*****************************************************
