#pragma once
#include <xwindows/XFrame.h>
#include <basic/PropertiesFile.h>
#include "AdvocatsListManager.h"

struct IDbConnection;
class CDbTable;

class CActsForm : public XFrame {
	CPropertiesFile props;

	std::shared_ptr<IDbConnection> conn;
	CAdvocatsListManager adv_list;

public:
	CActsForm(const Tchar *class_name, \
				const Tchar *label, const int X, const int Y, \
				const int width, const int height);
	virtual ~CActsForm();
};
