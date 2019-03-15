#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>

class CValidatedEdit;
class CDbGrid;

class CAdvocatsBook : public XWidget {
	CDbComboBox *flt_advocats;
	CValidatedEdit *flt_bdate;
	CDbComboBox *flt_district;
	CValidatedEdit *flt_license_no;
	CValidatedEdit *flt_license_date;
	CDbComboBox *flt_examiner;
	CValidatedEdit *flt_post_index;
	XEdit *flt_address;
	CValidatedEdit *flt_tel;
	CValidatedEdit *flt_email;
	XComboBox *flt_entr_type;
	XEdit *flt_entr_name;

	XButton *apply_filter, *ordering, *add, *remove, *upload;
	CDbGrid *grid;
	CDbNavigator *db_navigator;

	struct FilteringItem {
		std::string expression;
		IFilteringWidgetValueAdapter *value_holder;
		std::shared_ptr<IDbField> value_binder;
	};

	CSQLQueryFilter filter;
	std::vector<FilteringItem> filtering_items;
	void matchWidgetToFilter(XWidget *widget, const char *expression);
	void OnFilteringWidgetChanged(XCommandEvent *eve);

	std::shared_ptr<IDbConnection> createConnection(std::shared_ptr<CProperties> props);
	std::shared_ptr<CDbTable> createDbTable();
	void createCellWidgetsAndAttachToGrid(CEditableGrid *grid);
	CVerticalSizer createFilterWidgetsAndPackToSizer();

	void OnFilterButtonClick(XCommandEvent *eve);
	void OnOrderingButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);
	void OnUploadButtonClick(XCommandEvent *eve);
public:
	CAdvocatsBook();
	
	void Create(XWindow *parent, const int flags,
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	virtual ~CAdvocatsBook();
};
