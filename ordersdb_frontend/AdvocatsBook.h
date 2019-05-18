#include <basic/PropertiesFile.h>
#include <db_ext/FilteringManager.h>
#include <xwindows/XFrame.h>
#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>

struct IDbConnection;
class CDbTable;
class CDbComboBox;
class CDbGrid;

class CAdvocatsBook : public XFrame {
	enum Defaults {
		DEF_GUI_ROW_HEIGHT = 40, \
		DEF_GUI_VERT_GAP = 5, \
		DEF_GUI_HORZ_GAP = 5
	};

	CPropertiesFile props;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<CDbTable> db_table;
	CFilteringManager filtering_manager;

	CDbComboBox *flt_advocats;

	XButton *btn_apply_filter, *btn_ordering, *btn_add, *btn_remove, *btn_upload;
	CDbGrid *grid;

	void OnFilteringWidgetChanged(XCommandEvent *eve);

	std::shared_ptr<IDbConnection> createConnection(const CPropertiesFile &props);
	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);

	void OnFilterButtonClick(XCommandEvent *eve);
	void OnOrderingButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);
	void OnUploadButtonClick(XCommandEvent *eve);
public:
	CAdvocatsBook(const Tchar *class_name, \
					const Tchar *label, const int X, const int Y, \
					const int width, const int height);
	void DisplayWidgets();

	virtual ~CAdvocatsBook();
};
