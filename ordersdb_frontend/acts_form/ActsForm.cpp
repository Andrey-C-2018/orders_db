#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <xwindows/XButton.h>
#include "ActsForm.h"

CActsForm::CActsForm(const Tchar *class_name, \
						const Tchar *label, const int X, const int Y, \
						const int width, const int height) : \
				adv_list(DEF_GUI_MARGIN, ADV_LIST_WIDTH, DEF_DBNAVIGATOR_HEIGHT), \
				orders_list(DEF_GUI_MARGIN, 0.5F, DEF_DBNAVIGATOR_HEIGHT) {

	props.open("config.ini");
	
	conn = CMySQLConnectionFactory::createConnection(props);
	adv_list.initDbTable(conn);
	orders_list.initDbTable(conn);

	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	XRect rc;
	GetClientRect(rc);

	adv_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom));
	orders_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom), &adv_list.getFirstSizer());

	adv_list.displayWidgets(this);
	orders_list.displayWidgets(this);
	Connect(EVT_SIZE, this, &CActsForm::OnSize);
}

void CActsForm::OnSize(XSizeEvent *eve) {

	if(!adv_list.updateSizers(XSize(eve->GetWidth(), eve->GetHeight()))) return;
	orders_list.updateSizers();

	adv_list.resize();
	orders_list.resize();
}

CActsForm::~CActsForm() { }
