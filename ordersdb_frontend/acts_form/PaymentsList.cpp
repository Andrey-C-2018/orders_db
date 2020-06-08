#include <db_ext/DbTable.h>
#include <editable_grid/IntegerCellWidget.h>
#include <editable_grid/BooleanCellWidget.h>
#include <editable_grid/CurrencyCellWidget.h>
#include <editable_grid/DateCellWidget.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbNavigator.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <forms_common/ActNameCellWidget.h>
#include <forms_common/ActDateCellWidget.h>
#include <xwindows/XButton.h>
#include <forms_common/PaymentsGridEvtHandler.h>
#include "PaymentsList.h"

CPaymentsList::CPaymentsList(const int margins_, const int db_navigator_height_) : \
								grid(nullptr), grid_as_window(nullptr), \
								db_navigator(nullptr), panel(nullptr), \
								cr_grid(false), cr_navigator(false), cr_panel(false), \
								db_navigator_height(db_navigator_height_), \
								grid_sizer(margins_, 0), nav_sizer(margins_, margins_), \
								panel_sizer(margins_, margins_), prev_sizer(nullptr), \
								stages_list(nullptr), informers_list(nullptr), \
								checkers_list(nullptr) { }

void CPaymentsList::initDbTable(std::shared_ptr<IDbConnection> conn_, const int def_center, \
								const int def_order, CDate def_order_date, \
								std::shared_ptr<CPaymentsConstraints> constraints) {

	assert(!db_table);
	assert(!grid);
	assert(constraints);

	this->conn = conn_;
	db_table = createDbTable(conn_, def_center, def_order, def_order_date);
	grid = new CDbGrid(false, db_table, \
						std::make_shared<CPaymentsGridEvtHandler>(db_table, constraints));

	grid->SetFieldLabel(3, _T("Опл."));
	grid->SetFieldWidth(3, 4);
	grid->SetFieldLabel(5, _T("Стадія"));
	grid->SetFieldWidth(5, 14);
	grid->SetFieldLabel(6, _T("Цикл"));
	grid->SetFieldWidth(6, 4);
	grid->SetFieldLabel(7, _T("Стаття"));
	grid->SetFieldWidth(7, 20);
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
	panel = new CPaymentsNavPanel(conn_, db_table, constraints, \
									stages_list->getMasterResultSet(), \
									informers_list->getMasterResultSet(), \
									checkers_list->getMasterResultSet());
}

void CPaymentsList::initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler) {

	db_table->ConnectEventsHandler(evt_handler);
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
	db_table->markFieldAsPrimaryKey("id_center", "payments");
	db_table->markFieldAsPrimaryKey("id_order", "payments");
	db_table->markFieldAsPrimaryKey("order_date", "payments");
	db_table->markFieldAsPrimaryKey("cycle", "payments");
	db_table->markFieldAsPrimaryKey("id_stage", "payments");

	return db_table;
}

void CPaymentsList::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {
	enum { CYCLE_MAX_LEN = 3, QA_MAX_LEN = 1};

	assert(grid);
	CEditableCellWidget *is_paid_widget = nullptr;
	CIntegerCellWidget *cycle_widget = nullptr;
	CBooleanCellWidget *qa_widget = nullptr;
	CCurrencyCellWidget *currency_widget = nullptr, *koef_widget = nullptr;
	CDateCellWidget *date_widget = nullptr;
	CActNameCellWidget *act_name_widget = nullptr;
	CActDateCellWidget *act_date_widget = nullptr;

	bool is_paid = false, cycle = false, stages = false, fee = false;
	bool inf = false, act_name = false, date = false, chk = false, qa = false;
	bool act_date = false, koef = false;
	try {
		is_paid_widget = new CEditableCellWidget(true);
		grid->SetWidgetForFieldByName("is_paid", is_paid_widget);
		is_paid = true;

		cycle_widget = new CIntegerCellWidget(CYCLE_MAX_LEN);
		grid->SetWidgetForFieldByName("cycle", cycle_widget);
		cycle = true;

		stages_list = new CDbComboBoxCellWidget(conn, "stage_name", \
											"stages", "payments", db_table);
		stages_list->AddRelation("id_st", "id_stage");
		grid->SetWidgetForFieldByName("stage_name", stages_list);
		stages = true;

		currency_widget = new CCurrencyCellWidget(false);
		grid->SetWidgetForFieldByName("fee", currency_widget);
		fee = true;

		grid->SetWidgetForFieldByName("outgoings", currency_widget);
		
		informers_list = new CDbComboBoxCellWidget(conn, "informer_name", \
												"informers", "payments", db_table);
		informers_list->AddRelation("id_inf", "id_informer");
		grid->SetWidgetForFieldByName("informer_name", informers_list);
		inf = true;

		act_name_widget = new CActNameCellWidget();
		grid->SetWidgetForFieldByName("id_act", act_name_widget);
		act_name = true;

		date_widget = new CDateCellWidget(true);
		grid->SetWidgetForFieldByName("act_reg_date", date_widget);
		date = true;

		act_date_widget = new CActDateCellWidget(db_table);
		grid->SetWidgetForFieldByName("act_date", act_date_widget);
		act_date = true;

		qa_widget = new CBooleanCellWidget();
		grid->SetWidgetForFieldByName("age", qa_widget);
		qa = true;

		grid->SetWidgetForFieldByName("inv", qa_widget);
		grid->SetWidgetForFieldByName("lang", qa_widget);
		grid->SetWidgetForFieldByName("ill", qa_widget);
		grid->SetWidgetForFieldByName("zek", qa_widget);
		grid->SetWidgetForFieldByName("vpr", qa_widget);
		grid->SetWidgetForFieldByName("reduce", qa_widget);
		grid->SetWidgetForFieldByName("change_", qa_widget);
		grid->SetWidgetForFieldByName("close", qa_widget);
		grid->SetWidgetForFieldByName("zv", qa_widget);
		grid->SetWidgetForFieldByName("min", qa_widget);
		grid->SetWidgetForFieldByName("nm_suv", qa_widget);
		grid->SetWidgetForFieldByName("zv_kr", qa_widget);
		grid->SetWidgetForFieldByName("No_Ch_Ist", qa_widget);

		koef_widget = new CCurrencyCellWidget(true);
		grid->SetWidgetForFieldByName("Koef", koef_widget);
		koef = true;

		auto chk_stmt = conn->PrepareQuery("SELECT id_user,user_full_name FROM users WHERE user_full_name IS NOT NULL ORDER BY user_name");
		auto chk_rs = chk_stmt->exec();
		checkers_list = new CDbComboBoxCellWidget(conn, 1, chk_rs, \
													 chk_stmt->getResultSetMetadata(), \
													"payments", db_table);
		checkers_list->AddRelation("id_user", "id_checker");
		grid->SetWidgetForFieldByName("user_full_name", checkers_list);
		chk = true;
	}
	catch (...) {
		if (!chk && checkers_list && !checkers_list->IsCreated())
			delete checkers_list;

		if (!koef && koef_widget && !koef_widget->IsCreated())
			delete koef_widget;

		if (!qa && qa_widget && !qa_widget->IsCreated())
			delete qa_widget;

		if (!act_date && act_date && !act_date_widget->IsCreated())
			delete act_date_widget;

		if (!date && date_widget && !date_widget->IsCreated())
			delete date_widget;

		if (!inf && informers_list && !informers_list->IsCreated())
			delete informers_list;

		if (!fee && currency_widget && !currency_widget->IsCreated())
			delete currency_widget;

		if (!act_name && act_name_widget && !act_name_widget->IsCreated())
			delete act_name_widget;

		if (!stages && stages_list && !stages_list->IsCreated())
			delete stages_list;

		if (!cycle && cycle_widget && !cycle_widget->IsCreated())
			delete cycle_widget;

		if (!is_paid && is_paid_widget && !is_paid_widget->IsCreated())
			delete is_paid_widget;

		throw;
	}
}

void CPaymentsList::displayWidgets(XWindow *parent) {

	grid_sizer.createWidget(grid_as_window, parent, FL_WINDOW_VISIBLE, _T(""));
	cr_grid = true;
	nav_sizer.createWidget(db_navigator, parent, FL_WINDOW_VISIBLE, _T(""));
	cr_navigator = true;
	panel_sizer.createWidget(panel, parent, FL_WINDOW_VISIBLE, _T(""));
	cr_panel = true;

	grid->HideField(0);
	grid->HideField(1);
	grid->HideField(2);
	grid->HideField(4);
	grid->HideField(10);
	grid->HideField(31);
}

CPaymentsList::~CPaymentsList() {

	if (panel && !cr_panel) delete panel;
	if (db_navigator && !cr_navigator) delete db_navigator;
	if (grid && !cr_grid) delete grid;
}
