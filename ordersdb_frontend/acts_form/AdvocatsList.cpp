#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>
#include "AdvocatsList.h"
#include "AdvocatsListEvtHandler.h"

CAdvocatsList::CAdvocatsList(const int margins_, const int width_, \
								const int db_navigator_height_) : \
									grid(nullptr), grid_as_window(nullptr), \
									db_navigator(nullptr), \
									grid_sizer(margins_, 0), \
									nav_sizer(margins_, margins_),
									width(width_), db_navigator_height(db_navigator_height_){ }

void CAdvocatsList::initDbTable(std::shared_ptr<IDbConnection> conn_) {

	assert(!db_table);
	assert(!grid);

	db_table = createDbTable(conn_);
	grid = new CDbGrid(db_table, std::make_shared<CAdvocatsListEvtHandler>(db_table));
	grid->SetFieldLabel(1, _T("ПІБ адвоката"));
	grid->SetFieldWidth(1, 24);
	grid_as_window = grid;

	db_navigator = new CDbNavigator(db_table);
}

void CAdvocatsList::initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler) {

	db_table->ConnectEventsHandler(evt_handler);
}

std::shared_ptr<CDbTable> CAdvocatsList::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_advocat, b.adv_name_short ";
	query += "FROM advocats b ";
	query += "ORDER BY adv_name_short";

	auto stmt = conn->PrepareQuery(query.c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("advocats");
	db_table->markFieldAsPrimaryKey("id_advocat");

	db_table->gotoCurrentRecord();
	return db_table;
}

void CAdvocatsList::displayWidgets(XWindow *parent) {

	grid_sizer.createWidget(grid_as_window, parent, FL_WINDOW_VISIBLE, _T(""));
	nav_sizer.createWidget(db_navigator, parent, FL_WINDOW_VISIBLE, _T(""));

	grid->HideField(0);
}

CAdvocatsList::~CAdvocatsList() {

	if (db_navigator && !db_navigator->IsCreated()) delete db_navigator;
	if (grid && !grid->IsCreated()) delete grid;
}
