#include <basic/PropertiesFile.h>
#include <db_ext/FilteringManager.h>
#include <db_ext/QueryModifier.h>
#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>
#include <xwindows_ex/XTabStopFrame.h>
#include "AdvocatInserter.h"

struct IDbConnection;
class CDbTable;
class CDbGrid;
class CComboBoxCellWidget;
class CDbComboBoxCellWidget;
class CFilteringEdit;

class CAdvocatsBook : public XTabStopFrame {
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
	
	CDbGrid *grid;
	CDbComboBoxCellWidget *examiners_list, *districts_list;
	CComboBoxCellWidget *adv_org_types_list;
	int grid_x, grid_y;
	int grid_margin_x, grid_margin_y;

	XButton *btn_apply_filter, *btn_add, *btn_remove;
	CAdvocatInserter adv_inserter;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);
	void setFieldsSizes();
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);
	void adjustUIDependentCellWidgets(CDbGrid *grid);
	void initFilteringControls();

	void OnSize(XSizeEvent *eve);
	void OnFilterButtonClick(XCommandEvent *eve);
	void OnAddRecordButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);
public:
	CAdvocatsBook(const Tchar *class_name, \
					const Tchar *label, const int X, const int Y, \
					const int width, const int height);
	void DisplayWidgets();

	virtual ~CAdvocatsBook();
};
