#pragma once
#include <basic/PropertiesFile.h>
#include <xwindows/XFrame.h>
#include "AdvocatsList.h"
#include "OrdersList.h"
#include "PaymentsList.h"

struct IDbConnection;
class CDbTable;

class CActsForm : public XFrame {
	CPropertiesFile props;

	std::shared_ptr<IDbConnection> conn;
	CAdvocatsList adv_list;
	COrdersList orders_list;
	CPaymentsList payments_list;

	enum {
		DEF_GUI_MARGIN = 10, \
		ADV_LIST_WIDTH = 300, \
		DEF_DBNAVIGATOR_HEIGHT = 40
	};

	void OnSize(XSizeEvent *eve);
	void OnMaximizeOrRestore(XWindowButtonsEvent *eve);

public:
	CActsForm(const Tchar *class_name, \
				const Tchar *label, const int X, const int Y, \
				const int width, const int height);
	virtual ~CActsForm();
};

//*****************************************************