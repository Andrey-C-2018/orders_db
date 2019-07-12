#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>
#include "AdvocatsListManager.h"
#include "AdvocatsListEvtHandler.h"

CAdvocatsListManager::CAdvocatsListManager() : grid(nullptr), grid_as_window(nullptr), \
												grid_sizer(0, 10), nav_sizer(0, 0) { }

void CAdvocatsListManager::Init(std::shared_ptr<IDbConnection> conn_, XWindow *parent) {

	assert(!db_table);
	assert(!grid);

	db_table = createDbTable(conn_);
	grid = new CDbGrid(db_table, std::make_shared<CAdvocatsListEvtHandler>(db_table));
	grid->SetFieldLabel(1, _T("ПІБ адвоката"));
	grid->SetFieldWidth(1, 24);
		
	grid_as_window = grid;

	db_navigator = new CDbNavigator(db_table);
}

std::shared_ptr<CDbTable> CAdvocatsListManager::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_advocat, b.adv_name_short ";
	query += "FROM advocats b ";
	query += "ORDER BY adv_name_short";

	auto stmt = conn->PrepareQuery(query.c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("advocats");
	db_table->markFieldAsPrimaryKey("id_advocat");

	return db_table;
}

void CAdvocatsListManager::CreateWidgets(XWindow *parent, const int flags, \
										const Tchar *label, \
										const int x, const int y, \
										const int width, const int height) {

	grid_sizer.setCoords(XPoint(x, y));
	grid_sizer.setMinus(15);
	XRect rc = grid_sizer.resize(XSize(width, height));

	grid->Create(parent, flags, _T(""), rc.left, rc.top, \
				rc.right - rc.left, rc.bottom - rc.top);
	grid->HideField(0);

	rc = nav_sizer.resize(&grid_sizer);
	db_navigator->Create(parent, flags, _T(""), rc.left, rc.top, \
							rc.right - rc.left, rc.bottom - rc.top);
}

CAdvocatsListManager::~CAdvocatsListManager() {

	if (db_navigator && !db_navigator->IsCreated()) delete db_navigator;
	if (grid && !grid->IsCreated()) delete grid;
}
