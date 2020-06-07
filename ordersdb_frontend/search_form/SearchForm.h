#pragma once
#include <basic/PropertiesFile.h>
#include <db_ext/FilteringManager.h>
#include <db_ext/QueryModifier.h>
#include <xwindows_ex/XTabStopPanel.h>
#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>
#include "Inserter.h"

struct IDbConnection;
class CDbTable;
class CDbGrid;
class CDbStaticNumField;
class CComboBoxCellWidget;
class CDbComboBoxCellWidget;
class CFilteringEdit;
class CFilteringDbComboBox;
class CFilteringDateField;
class CZoneFilter;
class CPaidFilter;

class CSearchForm : public XTabStopPanel {
	enum Defaults {
		DEF_GUI_ROW_HEIGHT = 30, \
		DEF_GUI_VERT_GAP = 5, \
		DEF_GUI_HORZ_GAP = 5
	};

	CPropertiesFile props;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<CDbTable> db_table;

	CQueryModifier query_modifier, query_aggregate;
	CFilteringManager filtering_manager;
	CFilteringEdit *flt_id, *flt_act;
	CFilteringDateField *flt_order_date_from, *flt_order_date_to;
	CFilteringDateField *flt_act_reg_date_from, *flt_act_reg_date_to;
	CFilteringDateField *flt_act_date_from, *flt_act_date_to;
	CFilteringDateField *flt_payment_date_from, *flt_payment_date_to;
	CFilteringDbComboBox *flt_advocat, *flt_center, *flt_informer;
	CFilteringDbComboBox *flt_order_type, *flt_stage;
	CComboBoxCellWidget *canceling_reasons_list;
	CZoneFilter *flt_zone;
	CPaidFilter *flt_paid;

	CDbGrid *grid;
	CDbComboBoxCellWidget *advocats_list, *centers_list, *informers_list;
	CDbComboBoxCellWidget *order_types_list, *stages_list;
	int grid_x, grid_y;
	int grid_margin_x, grid_margin_y;

	CDbStaticNumField *total_fee, *total_paid, *total_orders;

	XButton *btn_apply_filter, *btn_add, *btn_remove;
	CInserter inserter;

	std::shared_ptr<CDbTable> createDbTable();
	void createStatisticsStatements();
	void reloadStatisticsControls();
	void reloadStatisticsControls(std::shared_ptr<IDbStatement> new_stmt);
	void setFieldsSizes();
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);
	void initFilteringControls();
	void displayWidgets();
	void adjustUIDependentCellWidgets();
	void loadInitialFilterToControls();

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
