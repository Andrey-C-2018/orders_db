#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbComboBoxCellWidget.h>

class CAdvocatsBook : public XWidget {
	CDbComboBoxCellWidget *flt_advocats;
	CValidatedEdit *flt_license_no;
	CValidatedEdit *flt_license_date;
	CDbComboBoxCellWidget *flt_examiner;
	CValidatedEdit *flt_post_index;
	XEdit *flt_address;
	CValidatedEdit *flt_tel;
	CValidatedEdit *flt_email;
	CComboBoxCellWidget *flt_entr_type;
	XEdit *flt_entr_name;

	XButton *filter, *sort, *add, *remove, *upload;
	CDbGrid *grid;
	
public:
	CAdvocatsBook();
	virtual ~CAdvocatsBook();
};