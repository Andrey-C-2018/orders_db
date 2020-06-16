#include <db_ext/DbTable.h>
#include <editable_grid/DateCellWidget.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>
#include <forms_common/ParametersManager.h>
#include "OrdersGridEvtHandler.h"
#include "OrdersList.h"

COrdersList::COrdersList(const int margins_, const float multiplier_, \
							const int db_navigator_height_) : \
								grid(nullptr), grid_as_window(nullptr), \
								db_navigator(nullptr), \
								multiplier(multiplier_), \
								db_navigator_height(db_navigator_height_), \
								cr_grid(false), cr_navigator(false), \
								grid_sizer(margins_, 0), nav_sizer(margins_, margins_), \
								prev_sizer(nullptr) { }

void COrdersList::initDbTable(std::shared_ptr<IDbConnection> conn_, \
								const int def_adv_id, \
							std::shared_ptr<CPaymentsConstraints> constraints) {

	assert(!db_table);
	assert(!grid);

	db_table = createDbTable(conn_, def_adv_id);
	grid = new CDbGrid(true, db_table, \
			std::make_shared<COrdersGridEvtHandler>(db_table, constraints));
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

	createCellWidgetsAndAttachToGrid(grid);

	db_navigator = new CDbNavigator(db_table);
}

void COrdersList::initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler) {

	db_table->ConnectDbEventsHandler(evt_handler);
}

std::shared_ptr<CDbTable> COrdersList::createDbTable(std::shared_ptr<IDbConnection> conn, \
														const int def_adv_id) {

	std::string query = "SELECT a.id_center_legalaid, cn.center_short_name, a.id, a.order_date,";
	query += " t.type_name, a.client_name, a.bdate, a.cancel_date ";
	query += "FROM orders a INNER JOIN centers cn ON a.id_center_legalaid = cn.id_center";
	query += " INNER JOIN order_types t ON a.id_order_type = t.id_type ";
	query += "WHERE ";
	query += CParametersManager::getInstance().getCenterFilteringStr();
	query += " AND a.id_adv = ? ";
	query += "ORDER BY id_center_legalaid, order_date, id";

	auto stmt = conn->PrepareQuery(query.c_str());
	stmt->bindValue(0, def_adv_id);

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("orders");
	db_table->markFieldAsPrimaryKey("id_center_legalaid", "orders");
	db_table->markFieldAsPrimaryKey("id", "orders");
	db_table->markFieldAsPrimaryKey("order_date", "orders");

	if(!db_table->empty())
		db_table->gotoCurrentRecord();
	return db_table;
}

void COrdersList::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {

	assert(grid);
	CDateCellWidget *date_widget = nullptr;
	CEditableCellWidget *client_widget = nullptr;
	bool cli = false, date = false;
	try {
		client_widget = new CEditableCellWidget(false);
		grid->SetWidgetForFieldByName("client_name", client_widget);
		cli = true;

		date_widget = new CDateCellWidget();
		grid->SetWidgetForFieldByName("bdate", date_widget);
		date = true;

		grid->SetWidgetForFieldByName("cancel_date", date_widget);
	}
	catch (...) {
		if (!date && date_widget && !date_widget->IsCreated())
			delete date_widget;

		if (!cli && client_widget && !client_widget->IsCreated())
			delete client_widget;

		throw;
	}
}

void COrdersList::displayWidgets(XWindow *parent) {

	grid_sizer.createWidget(grid_as_window, parent, FL_WINDOW_VISIBLE, _T(""));
	cr_grid = true;
	nav_sizer.createWidget(db_navigator, parent, FL_WINDOW_VISIBLE, _T(""));
	cr_navigator = true;

	grid->HideField(0);
}

COrdersList::~COrdersList() {

	if (db_navigator && !cr_navigator) delete db_navigator;
	if (grid && !cr_grid) delete grid;
}