#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include "ActsForm.h"

CActsForm::CActsForm(const Tchar *class_name, \
						const Tchar *label, const int X, const int Y, \
						const int width, const int height) : \
				adv_sizer(DEF_GUI_MARGIN, DEF_GUI_MARGIN) {

	props.open("config.ini");
	
	conn = CMySQLConnectionFactory::createConnection(props);
	adv_list.Init(conn, this);

	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	XRect rc;
	GetClientRect(rc);

	adv_sizer.setCoords(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN));
	rc = adv_sizer.resize(XSize(ADV_LIST_WIDTH, rc.bottom));

	adv_list.CreateWidgets(this, FL_WINDOW_VISIBLE, _T(""), \
							rc.left, rc.top, \
							rc.right - rc.left, rc.bottom - rc.top);

	Connect(EVT_SIZE, this, &CActsForm::OnSize);
}

void CActsForm::OnSize(XSizeEvent *eve) {

	XSize left_part_size(ADV_LIST_WIDTH, eve->GetHeight());
	//XSize right_part_size(eve->GetWidth() - 100, eve->GetHeight());;
	
	adv_list.resize(adv_sizer.resize(left_part_size));

}

CActsForm::~CActsForm() { }
