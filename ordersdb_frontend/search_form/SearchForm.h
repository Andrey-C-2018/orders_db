#pragma once
#include <set>
#include <db_ext/FilteringManager.h>
#include <db_ext/SortingManager.h>
#include <db_ext/QueryModifier.h>
#include <xwindows_ex/XTabStopPanel.h>
#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>
#include <xwindows/XProgressBar.h>
#include <forms_common/FlexiblePropepties.h>
#include "Inserter.h"
#include "UploadDialog.h"
#include "FieldsList.h"

struct IDbConnection;
class CDoubleBndTarget;
class CDbTable;
class CDbGrid;
struct CPaymentsConstraints;
class CDbStaticNumField;
class CComboBoxCellWidget;
class CDbComboBoxCellWidget;
class CBooleanCellWidget;
class CCurrencyCellWidget;
class CFilteringEdit;
class CFilteringDbComboBox;
class CFilteringDateField;
class CZoneFilter;
class CPaidFilter;
class CActNoFilter;
class COrderingComboBox;

class CSearchForm : public XTabStopPanel {
	enum Defaults {
		DEF_GUI_ROW_HEIGHT = 30, \
		DEF_GUI_VERT_GAP = 5, \
		DEF_GUI_HORZ_GAP = 5, \
		FIELDS_COUNT = 37
	};

	CFlexiblePropepties props;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<CDbTable> db_table;
	std::shared_ptr<CPaymentsConstraints> constraints;

	CQueryModifier query_modifier, query_aggregate;
	CFilteringManager filtering_manager;
	CSortingManager sorting_manager;
	CFilteringEdit *flt_id, *flt_act, *flt_client;
	CFilteringDateField *flt_order_date_from, *flt_order_date_to;
	CFilteringDateField *flt_act_reg_date_from, *flt_act_reg_date_to;
	CFilteringDateField *flt_act_date_from, *flt_act_date_to;
	CFilteringDateField *flt_payment_date_from, *flt_payment_date_to;
	CFilteringDbComboBox *flt_advocat, *flt_center, *flt_informer;
	CFilteringDbComboBox *flt_order_type, *flt_stage;
	CComboBoxCellWidget *canceling_reasons_list, *act_no_list;
	CZoneFilter *flt_zone;
	CPaidFilter *flt_paid;
	CActNoFilter *flt_act_no;

	CDbGrid *grid;
	CDbComboBoxCellWidget *advocats_list, *centers_list, *informers_list;
	CDbComboBoxCellWidget *order_types_list, *stages_list, *checkers_list;
	CBooleanCellWidget *qa_widget;
	int grid_x, grid_y;
	int grid_margin_x, grid_margin_y;

	CDbStaticNumField *total_fee, *total_paid, *total_orders;
	std::shared_ptr<CDoubleBndTarget> def_binding_target;

	XButton *btn_apply_filter, *btn_add, *btn_remove, *btn_rev;
	XButton *btn_reset, *btn_sort, *btn_upload;
	CInserter inserter;
	UploadDialog uploader;
	XProgressBar *progress;
	FieldsList fields_list;

	std::shared_ptr<IDbStatement> createTableStmt();
	std::shared_ptr<CDbTable> createDbTable();
	void createStatisticsStatements();
	void reloadStatisticsControls();
	void reloadStatisticsControls(std::shared_ptr<IDbStatement> new_stmt);
	void createDbGrid(std::shared_ptr<CPaymentsConstraints> constraints);
	void setFieldsSizes();
	void createCellWidgetsAndAttachToGrid(const bool db_admin);
	void initFilteringControls();
	void displayWidgets();
	void adjustUIDependentCellWidgets();
	void loadInitialFilterToControls();
	void initOrdering(COrderingComboBox *ordering_box);

	void OnFilterButtonClick(XCommandEvent *eve);
	void OnAddRecordButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);
	void OnRevButtonClick(XCommandEvent *eve);
	void OnResetButtonClick(XCommandEvent *eve);
	void OnSortButtonClick(XCommandEvent *eve);
	void OnUploadButtonClick(XCommandEvent *eve);
	
public:
	CSearchForm(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	void OnSize(XSizeEvent *eve);

	virtual ~CSearchForm();
};

//*****************************************************
