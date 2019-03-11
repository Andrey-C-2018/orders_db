#include "MyFrame.h"
#include <db_ext/DbTable.h>
#include <db/MySQL/MySQLConnection.h>
#include <db_controls/DbComboBoxCellWidget.h>

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) : grid(nullptr){

	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	Connect(EVT_SIZE, this, &CMyFrame::OnSize);

	XRect rc;
	GetClientRect(rc);

	auto db_table = initDbTable();

	CDbComboBoxCellWidget *combo = new CDbComboBoxCellWidget(conn, 1, "examiners", "advocats", db_table);
	combo->AddRelation("id_examiner", "id_exam");

	grid = new CDbGrid(db_table);
	grid->SetWidgetForField(4, combo);

	grid->Create(this, FL_WINDOW_VISIBLE, _T(""), 20, 40, \
					rc.right - 40, rc.bottom - 60);
	grid->SetFocus();

}

std::shared_ptr<CDbTable> CMyFrame::initDbTable() {

	std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();

	conn->Connect("127.0.0.1", 3310, "orders_user", "123", "orders_test");

	std::string query = "SELECT b.id_advocat, b.adv_name, b.license_no, b.license_date, e.examiner_name ";
	query += "FROM advocats b INNER JOIN examiners e ON b.id_exm = a.id_examiner ";
	query += "ORDER BY adv_name";

	auto stmt = conn->PrepareQuery(query.c_str());
	
	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("advocats");
	db_table->markFieldAsPrimaryKey(0);

	return db_table;
}

CMyFrame::~CMyFrame(){ }
