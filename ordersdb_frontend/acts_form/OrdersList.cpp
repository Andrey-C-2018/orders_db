#include "OrdersList.h"
#include <db_ext/DbTable.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>

COrdersList::COrdersList(const int margins_, const float multiplier_, \
							const int db_navigator_height_) : \
								grid(nullptr), grid_as_window(nullptr), \
								db_navigator(nullptr), \
								multiplier(multiplier_), \
								db_navigator_height(db_navigator_height_), \
								grid_sizer(margins_, 0), nav_sizer(margins_, margins_), \
								prev_sizer(nullptr) { }

void COrdersList::initDbTable(std::shared_ptr<IDbConnection> conn_) {

	assert(!db_table);
	assert(!grid);

	db_table = createDbTable(conn_);
	grid = new CDbGrid(db_table);
	grid->SetFieldLabel(1, _T("Центр"));
	grid->SetFieldWidth(1, 12);
	grid->SetFieldLabel(2, _T("№"));
	grid->SetFieldWidth(2, 5);
	grid->SetFieldLabel(3, _T("Дата"));
	grid->SetFieldLabel(4, _T("Тип"));
	grid->SetFieldWidth(4, 6);
	grid->SetFieldLabel(5, _T("Клієнт"));
	grid->SetFieldWidth(5, 40);
	grid->SetFieldLabel(6, _T("Дата нар."));
	grid->SetFieldLabel(7, _T("Дата скс."));
	grid_as_window = grid;

	db_navigator = new CDbNavigator(db_table);
}

std::shared_ptr<CDbTable> COrdersList::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT a.id_center_legalaid, cn.center_short_name, a.id, a.order_date,";
	query += " t.type_name, a.client_name, a.bdate, a.cancel_date ";
	query += "FROM orders a INNER JOIN centers cn ON a.id_center_legalaid = cn.id_center";
	query += " INNER JOIN order_types t ON a.id_order_type = t.id_type ";
	query += "WHERE a.order_date >= '2019-01-01' ";
	query += "ORDER BY id_center_legalaid, order_date, id";

	auto stmt = conn->PrepareQuery(query.c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("orders");
	db_table->markFieldAsPrimaryKey("id_center_legalaid");
	db_table->markFieldAsPrimaryKey("id");
	db_table->markFieldAsPrimaryKey("order_date");

	return db_table;
}

void COrdersList::displayWidgets(XWindow *parent) {

	grid_sizer.createWidget(grid_as_window, parent, FL_WINDOW_VISIBLE, _T(""));
	nav_sizer.createWidget(db_navigator, parent, FL_WINDOW_VISIBLE, _T(""));

	grid->HideField(0);
}

COrdersList::~COrdersList() {

	if (db_navigator && !db_navigator->IsCreated()) delete db_navigator;
	if (grid && !grid->IsCreated()) delete grid;
}