#pragma once
#include <basic/PropertiesFile.h>
#include <xwindows/XFrame.h>
#include <xwindows/XDynamicSizer.h>
#include "AdvocatsListManager.h"

struct IDbConnection;
class CDbTable;

class CActsForm : public XFrame {
	CPropertiesFile props;

	std::shared_ptr<IDbConnection> conn;
	CAdvocatsListManager adv_list;

	enum {
		DEF_GUI_MARGIN = 10, \
		ADV_LIST_WIDTH = 300
	};

	XDynamicSizer adv_sizer, orders_sizer, payments_sizer;
	void OnSize(XSizeEvent *eve);

public:
	CActsForm(const Tchar *class_name, \
				const Tchar *label, const int X, const int Y, \
				const int width, const int height);
	virtual ~CActsForm();
};
