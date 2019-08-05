#include "PaymentsList.h"
#include <db_ext/DbTable.h>
#include <editable_grid/CurrencyCellWidget.h>
#include <editable_grid/DateCellWidget.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>
#include <xwindows/XButton.h>

CPaymentsList::CPaymentsList(const int margins_, const int db_navigator_height_) : \
								grid(nullptr), grid_as_window(nullptr), \
								db_navigator(nullptr), panel(nullptr), \
								db_navigator_height(db_navigator_height_), \
								grid_sizer(margins_, 0), nav_sizer(margins_, margins_), \
								panel_sizer(margins_, margins_), prev_sizer(nullptr) { }

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
	grid->SetFieldLabel(11, _T("Інформатор"));
	grid->SetFieldWidth(11, 30);
	grid->SetFieldLabel(12, _T("Акт"));
	grid->SetFieldWidth(12, 9);
	grid->SetFieldLabel(13, _T("Дата акту"));
	grid->SetFieldLabel(14, _T("Дт. реєстр"));

	grid->SetFieldLabel(15, _T("Вік"));
	grid->SetFieldLabel(16, _T("Вади"));
	grid->SetFieldWidth(16, 5);
	grid->SetFieldLabel(17, _T("Мова"));
	grid->SetFieldWidth(17, 5);
	grid->SetFieldLabel(18, _T("Хв"));
	grid->SetFieldLabel(19, _T("ЗЕК"));
	grid->SetFieldLabel(20, _T("Випр"));
	grid->SetFieldWidth(20, 4);
	grid->SetFieldLabel(21, _T("Зменш"));
	grid->SetFieldWidth(21, 6);
	grid->SetFieldLabel(22, _T("Змін"));
	grid->SetFieldWidth(22, 4);
	grid->SetFieldLabel(23, _T("Закр"));
	grid->SetFieldWidth(23, 4);
	grid->SetFieldLabel(24, _T("Звільн"));
	grid->SetFieldWidth(24, 6);
	grid->SetFieldLabel(25, _T("Мін."));
	grid->SetFieldLabel(26, _T("Найм. сув."));
	grid->SetFieldWidth(26, 10);
	grid->SetFieldLabel(27, _T("Звільн. кр."));
	grid->SetFieldWidth(27, 10);
	grid->SetFieldLabel(28, _T("Без зм. 1 інст"));
	grid->SetFieldWidth(28, 12);
	grid->SetFieldLabel(29, _T("Кзвіт"));
	grid->SetFieldWidth(29, 5);
	grid->SetFieldLabel(30, _T("Перевірив"));
	grid->SetFieldWidth(30, 20);

	grid_as_window = grid;
	createCellWidgetsAndAttachToGrid(grid);

	db_navigator = new CDbNavigator(db_table);
	panel = new CPaymentsNavPanel(conn_);
	panel->setPaymentsDbTable(db_table);
}

std::shared_ptr<CDbTable> CPaymentsList::createDbTable(std::shared_ptr<IDbConnection> conn, \
														const int def_center, \
														const int def_order, \
														CDate def_order_date) {

	std::string query = "SELECT aa.id_center, aa.id_order, aa.order_date,";
	query += " aa.is_paid, aa.id_stage, st.stage_name, aa.cycle, aa.article, aa.fee, aa.outgoings,";
	query += " aa.id_informer, inf.informer_name, aa.id_act, aa.act_date, aa.act_reg_date,";
	query += " aa.age,aa.inv,aa.lang,aa.ill,aa.zek,aa.vpr,aa.reduce,aa.change_,";
	query += " aa.close,aa.zv,aa.min,aa.nm_suv,aa.zv_kr,aa.No_Ch_Ist,aa.Koef,u.user_full_name,aa.id_checker ";
	query += "FROM payments aa INNER JOIN stages st ON aa.id_stage = st.id_st";
	query += " INNER JOIN informers inf ON aa.id_informer = inf.id_inf";
	query += " INNER JOIN users u ON aa.id_checker = u.id_user ";
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

void CPaymentsList::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {

	assert(grid);
	CCurrencyCellWidget *currency_widget = nullptr;
	CDateCellWidget *date_widget = nullptr;
	bool fee = false, date = false;
	try {
		currency_widget = new CCurrencyCellWidget();
		grid->SetWidgetForFieldByName("fee", currency_widget);
		fee = true;

		grid->SetWidgetForFieldByName("outgoings", currency_widget);

		date_widget = new CDateCellWidget();
		grid->SetWidgetForFieldByName("act_reg_date", date_widget);
		date = true;

		grid->SetWidgetForFieldByName("act_date", date_widget);
	}
	catch (...) {
		if (!date && date_widget && !date_widget->IsCreated())
			delete date_widget;

		throw;
	}
}

void CPaymentsList::displayWidgets(XWindow *parent) {

	grid_sizer.createWidget(grid_as_window, parent, FL_WINDOW_VISIBLE, _T(""));
	nav_sizer.createWidget(db_navigator, parent, FL_WINDOW_VISIBLE, _T(""));
	panel_sizer.createWidget(panel, parent, FL_WINDOW_VISIBLE, _T(""));

	grid->HideField(0);
	grid->HideField(1);
	grid->HideField(2);
	grid->HideField(4);
	grid->HideField(10);
	grid->HideField(31);
}

CPaymentsList::~CPaymentsList() {

	if (panel && !panel->IsCreated()) delete panel;
	if (db_navigator && !db_navigator->IsCreated()) delete db_navigator;
	if (grid && !grid->IsCreated()) delete grid;
}
