#include "MyFrame.h"
#include <db_ext/DbTable.h>
#include <db/DbConnectionFactory.h>

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) : grid(nullptr){
Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
		label, X, Y, width, height);

Connect(EVT_SIZE, this, &CMyFrame::OnSize);

XRect rc;
GetClientRect(rc);

const CDbConnectionFactory &factory = CDbConnectionFactory::getInstance();
std::shared_ptr<IDbConnection> conn;

conn = factory.createConnection(CDbConnectionFactory::DB_CONN_MYSQL);
conn->Connect("127.0.0.1", 3310, "orders_user", "123", "orders");

auto stmt = conn->PrepareQuery("SELECT id_center_legalaid, id, order_date FROM orders WHERE id = 1216");
auto rowset = stmt->exec();
rowset->gotoRecord(0);

auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));

grid = new CDbGrid(db_table);

grid->Create(this, FL_WINDOW_VISIBLE, _T(""), 20, 20, \
				rc.right - 40, rc.bottom - 40);
grid->SetFocus();

button_ce = new XButton(this, FL_WINDOW_VISIBLE, \
						_T("CE"), 10, 10, 10, 10);
button_ce2 = new XButton(this, FL_WINDOW_VISIBLE, \
	_T("CE2"), 10, 30, 10, 10);
Connect(EVT_COMMAND, button_ce->GetId(), this, &CMyFrame::OnButtonCeClick);
Connect(EVT_COMMAND, button_ce2->GetId(), this, &CMyFrame::OnButtonCe2Click);
}

void CMyFrame::OnButtonCeClick(XCommandEvent *eve) {

	grid->SetFocus();
	//grid->SetFieldWidth(4, 50);
	grid->HideField(13);
}

void CMyFrame::OnButtonCe2Click(XCommandEvent *eve) {

	grid->SetFocus();
	//grid->SetFieldWidth(4, 50);
	grid->HideField(11);
}

void CMyFrame::OnSize(XSizeEvent *eve) {
int width = eve->GetWidth();
int height = eve->GetHeight();

if (grid) grid->MoveWindow(20, 20, width - 40, height - 40);
}

CMyFrame::~CMyFrame(){ }