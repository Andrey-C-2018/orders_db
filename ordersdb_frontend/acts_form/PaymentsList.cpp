#include "PaymentsList.h"
#include <db_ext/DbTable.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>

CPaymentsList::CPaymentsList(const int margins_, const int db_navigator_height_) : \
								grid(nullptr), grid_as_window(nullptr), \
								db_navigator(nullptr), \
								db_navigator_height(db_navigator_height_), \
								grid_sizer(margins_, 0), nav_sizer(margins_, margins_), \
								prev_sizer(nullptr) { } 

void CPaymentsList::initDbTable(std::shared_ptr<IDbConnection> conn_, const int def_center, \
								const int def_order, CDate def_order_date) {

	assert(!db_table);
	assert(!grid);

	db_table = createDbTable(conn_, def_center, def_order, def_order_date);
	grid = new CDbGrid(db_table);
	grid->SetFieldLabel(3, _T("Опл."));
	grid->SetFieldWidth(3, 4);
	grid->SetFieldLabel(5, _T("Стадія"));
	grid->SetFieldWidth(5, 14);
	grid->SetFieldLabel(6, _T("Цикл"));
	grid->SetFieldWidth(6, 4);
	grid->SetFieldLabel(7, _T("Стаття"));
	grid->SetFieldWidth(7, 30);
	grid->SetFieldLabel(8, _T("Сума"));
	grid->SetFieldWidth(8, 8);
	grid->SetFieldLabel(9, _T("Витрати"));
	grid->SetFieldWidth(9, 8);
	grid->SetFieldLabel(10, _T("Інформатор"));
	grid->SetFieldWidth(10, 30);
	grid->SetFieldLabel(11, _T("Акт"));
	grid->SetFieldWidth(11, 9);
	grid->SetFieldLabel(12, _T("Дата акту"));
	grid->SetFieldLabel(13, _T("Дт. реєстр"));

	grid_as_window = grid;

	db_navigator = new CDbNavigator(db_table);
}

std::shared_ptr<CDbTable> CPaymentsList::createDbTable(std::shared_ptr<IDbConnection> conn, \
														const int def_center, \
														const int def_order, \
														CDate def_order_date) {

	std::string query = "SELECT aa.id_center, aa.id_order, aa.order_date,";
	query += " aa.is_paid, aa.id_stage, st.stage_name, aa.cycle, aa.article, aa.fee, aa.outgoings,";
	query += " inf.informer_name, aa.id_act, aa.act_date, aa.act_reg_date ";
	query += "FROM payments aa INNER JOIN stages st ON aa.id_stage = st.id_st";
	query += " INNER JOIN informers inf ON aa.id_informer = inf.id_inf ";
	query += "WHERE aa.id_center = ? AND aa.id_order = ? AND aa.order_date = ? ";
	query += "ORDER BY id_center, order_date, id_order, cycle, id_stage";

	auto stmt = conn->PrepareQuery(query.c_str());
	stmt->bindValue(0, def_center);
	stmt->bindValue(1, def_order);
	stmt->bindValue(2, def_order_date);

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("payments");
	db_table->markFieldAsPrimaryKey("id_center");
	db_table->markFieldAsPrimaryKey("id_order");
	db_table->markFieldAsPrimaryKey("order_date");
	db_table->markFieldAsPrimaryKey("cycle");
	db_table->markFieldAsPrimaryKey("id_stage");

	return db_table;
}

void CPaymentsList::displayWidgets(XWindow *parent) {

	grid_sizer.createWidget(grid_as_window, parent, FL_WINDOW_VISIBLE, _T(""));
	nav_sizer.createWidget(db_navigator, parent, FL_WINDOW_VISIBLE, _T(""));

	grid->HideField(0);
	grid->HideField(1);
	grid->HideField(2);
	grid->HideField(4);
}

CPaymentsList::~CPaymentsList() {

	if (db_navigator && !db_navigator->IsCreated()) delete db_navigator;
	if (grid && !grid->IsCreated()) delete grid;
}
