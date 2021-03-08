#pragma once
#include <xwindows_ex/XTabStopPanel.h>
#include <forms_common/FlexiblePropepties.h>
#include "AdvocatsList.h"
#include "OrdersList.h"
#include "PaymentsList.h"

struct IDbConnection;
class CDbTable;
struct CPaymentsConstraints;

class CActsForm : public XTabStopPanel {
	CFlexiblePropepties props;

	std::shared_ptr<IDbConnection> conn;
	CAdvocatsList adv_list;
	COrdersList orders_list;
	CPaymentsList payments_list;
	std::shared_ptr<CPaymentsConstraints> constraints;

	enum {
		DEF_GUI_MARGIN = 10, \
		ADV_LIST_WIDTH = 220, \
		DEF_DBNAVIGATOR_HEIGHT = 40
	};

public:
	CActsForm(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	void OnSize(XSizeEvent *eve);

	virtual ~CActsForm();
};

//*****************************************************