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
class CFilteringEdit;

class CSearchForm : public XTabStopPanel {
	enum {
		DEF_GUI_MARGIN = 10, \
		DEF_DBNAVIGATOR_HEIGHT = 40
	};

	CPropertiesFile props;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<CDbTable> db_table;

	CQueryModifier query_modifier;
	CFilteringManager filtering_manager;
	CFilteringEdit *flt_id;

	CDbGrid *grid;

	XButton *btn_apply_filter, *btn_add, *btn_remove;
	//CAdvocatInserter adv_inserter;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);
	void setFieldsSizes();
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);
	void displayInputWidgets();
	void adjustUIDependentCellWidgets(CDbGrid *grid);
	void initFilteringControls();

	void OnFilterButtonClick(XCommandEvent *eve);
	void OnAddRecordButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);
	void OnSize(XSizeEvent *eve);

public:
	CSearchForm(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	

	virtual ~CSearchForm();
};

//*****************************************************
