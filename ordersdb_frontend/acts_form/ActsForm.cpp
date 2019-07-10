#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include "ActsForm.h"

CActsForm::CActsForm(const Tchar *class_name, \
						const Tchar *label, const int X, const int Y, \
						const int width, const int height) {

	props.open("config.ini");
	
	conn = CMySQLConnectionFactory::createConnection(props);
	adv_list.Init(conn, this);

	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	adv_list.CreateWidgets(this, FL_WINDOW_VISIBLE, _T(""), 10, 10, 400, 300);
}

CActsForm::~CActsForm() { }
