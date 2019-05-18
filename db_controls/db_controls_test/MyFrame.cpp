#include "MyFrame.h"
#include <db_ext/DbTable.h>
#include <db/MySQL/MySQLConnection.h>
//#include <db/SQLite/SQLiteConnection.h>
#include <db_controls/DbComboBoxCellWidget.h>

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) : grid(nullptr){

	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	Connect(EVT_SIZE, this, &CMyFrame::OnSize);

	XRect rc;
	GetClientRect(rc);

	std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();
	//std::shared_ptr<IDbConnection> conn = std::make_shared<SQLiteConnection>();

	conn->Connect("192.168.1.2", 3310, "orders_user", "123", "orders");
	//conn->Connect("c:\\#projects\\test", 0, nullptr, nullptr, "test");

	auto stmt = conn->PrepareQuery("SELECT id_center_legalaid, id, order_date, type_name FROM orders INNER JOIN order_types ON id_order_type = id_type WHERE id BETWEEN 1700 AND 1702 ORDER BY 1,3,2");
	//auto stmt = conn->PrepareQuery("SELECT id_center, id, order_date, type_name FROM orders INNER JOIN order_types ON id_order_type = id_type ORDER BY 1,3,2");
	
	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("orders");
	db_table->markFieldAsPrimaryKeyByIndex(0);
	db_table->markFieldAsPrimaryKeyByIndex(1);
	db_table->markFieldAsPrimaryKeyByIndex(2);
	CDbComboBoxCellWidget *combo = new CDbComboBoxCellWidget(conn, 1, "order_types", "orders", db_table);
	combo->AddRelation("id_type", "id_order_type");

	grid = new CDbGrid(db_table);
	grid->SetWidgetForField(3, combo);

	grid->Create(this, FL_WINDOW_VISIBLE, _T(""), 20, 40, \
					rc.right - 40, rc.bottom - 60);
	grid->SetFocus();

	button_focus = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Focus"), 20, 10, 50, 20);
	button_add = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Add"), 80, 10, 40, 20);
	button_remove = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Remove"), 130, 10, 65, 20);
	button_refresh = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Refresh"), 205, 10, 70, 20);

	Connect(EVT_COMMAND, button_focus->GetId(), this, \
			&CMyFrame::OnButtonFocusClick);
	Connect(EVT_COMMAND, button_add->GetId(), this, \
			&CMyFrame::OnButtonAddClick);
	Connect(EVT_COMMAND, button_remove->GetId(), this, \
			&CMyFrame::OnButtonRemoveClick);
	Connect(EVT_COMMAND, button_refresh->GetId(), this, \
			&CMyFrame::OnButtonRefreshClick);
}

void CMyFrame::OnSize(XSizeEvent *eve) {

	int width = eve->GetWidth();
	int height = eve->GetHeight();

	if (grid) grid->MoveWindow(20, 40, width - 40, height - 60);
}

void CMyFrame::OnButtonFocusClick(XCommandEvent *eve) {

	grid->SetFocus();
}

void CMyFrame::OnButtonAddClick(XCommandEvent *eve) { }

void CMyFrame::OnButtonRemoveClick(XCommandEvent *eve) { }

void CMyFrame::OnButtonRefreshClick(XCommandEvent *eve) {

	grid->getDbTable()->reload();
}

CMyFrame::~CMyFrame(){ }