#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>
#include <forms_common/ParametersManager.h>
#include "DefendersList.h"
#include "DefendersListEvtHandler.h"

DefendersList::DefendersList(const int margins_, const int width_, \
								const int db_navigator_height_) : \
									grid(nullptr), grid_as_window(nullptr), \
									db_navigator(nullptr), \
									cr_grid(false), cr_navigator(false), \
									grid_sizer(margins_, 0), \
									nav_sizer(margins_, margins_),
									width(width_), db_navigator_height(db_navigator_height_){ }

void DefendersList::initDbTable(std::shared_ptr<IDbConnection> conn_) {

	assert(!db_table);
	assert(!grid);

	db_table = createDbTable(conn_);
	grid = new CDbGrid(true, db_table, std::make_shared<DefendersListEvtHandler>(db_table));
	grid->SetFieldLabel(1, _T("ПІБ захисника"));
	grid->SetFieldWidth(1, 24);
	grid_as_window = grid;

	db_navigator = new CDbNavigator(db_table);
}

void DefendersList::initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler) {

	db_table->ConnectDbEventsHandler(evt_handler);
}

std::shared_ptr<CDbTable> DefendersList::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT p.id_person, p.name_short ";
	query += "FROM people p ";
	query += "WHERE p.id_person IN (SELECT DISTINCT a.id_defender FROM orders a WHERE ";
	query += CParametersManager::getInstance().getCenterFilteringStr();
	query += ") ";
	query += "ORDER BY p.name_short";

	auto stmt = conn->PrepareQuery(query.c_str());

	auto db_table = std::make_shared<CDbTable>(conn, stmt);
	db_table->setPrimaryTableForQuery("people");
	db_table->markFieldAsPrimaryKey("id_person", "people");

	db_table->gotoCurrentRecord();
	return db_table;
}

void DefendersList::displayWidgets(XWindow *parent) {

	grid_sizer.createWidget(grid_as_window, parent, FL_WINDOW_VISIBLE, _T(""));
	cr_grid = true;
	nav_sizer.createWidget(db_navigator, parent, FL_WINDOW_VISIBLE, _T(""));
	cr_navigator = true;

	grid->HideField(0);
}

DefendersList::~DefendersList() {

	if (db_navigator && !cr_navigator) delete db_navigator;
	if (grid && !cr_grid) delete grid;
}
