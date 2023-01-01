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
#include "PaymentsGridEvtHandler.h"
#include "PaymentsList.h"

CPaymentsList::CPaymentsList(const int margins_, const int db_navigator_height_) : \
								grid(nullptr), grid_as_window(nullptr), \
								db_navigator(nullptr), panel(nullptr), \
								cr_grid(false), cr_navigator(false), cr_panel(false), \
								db_navigator_height(db_navigator_height_), \
								grid_sizer(margins_, 0), nav_sizer(margins_, margins_), \
								panel_sizer(margins_, margins_), prev_sizer(nullptr), \
								stages_list(nullptr), informers_list(nullptr), \
								checkers_list(nullptr), act_no_widget(nullptr) { }

void CPaymentsList::initDbTable(std::shared_ptr<IDbConnection> conn_, const int def_center, \
								const int def_order, CDate def_order_date, \
								std::shared_ptr<CPaymentsConstraints> constraints) {

	assert(!db_table);
	assert(!grid);
	assert(constraints);

	this->conn = conn_;
	db_table = createDbTable(conn_, def_center, def_order, def_order_date);

	const auto &meta_info = db_table->getQuery().getMetaInfo();
	grid_evt_handler = std::make_shared<CPaymentsGridEvtHandler>(db_table, \
																	constraints);
	grid = new CDbGrid(false, db_table, grid_evt_handler);

	size_t field_index = meta_info.getFieldIndexByName("is_paid");
	grid->SetFieldLabel(field_index, _T("Опл."));
	grid->SetFieldWidth(field_index, 4);

	field_index = meta_info.getFieldIndexByName("stage_name");
	grid->SetFieldLabel(field_index, _T("Стадія"));
	grid->SetFieldWidth(field_index, 14);

	field_index = meta_info.getFieldIndexByName("cycle");
	grid->SetFieldLabel(field_index, _T("#"));
	grid->SetFieldWidth(field_index, 2);

	field_index = meta_info.getFieldIndexByName("article");
	grid->SetFieldLabel(field_index, _T("Стаття"));
	grid->SetFieldWidth(field_index, 20);

	field_index = meta_info.getFieldIndexByName("fee");
	grid->SetFieldLabel(field_index, _T("Сума"));
	grid->SetFieldWidth(field_index, 8);

	field_index = meta_info.getFieldIndexByName("outg_post");
	grid->SetFieldLabel(field_index, _T("Пошт"));
	grid->SetFieldWidth(field_index, 5);

	field_index = meta_info.getFieldIndexByName("outg_daynight");
	grid->SetFieldLabel(field_index, _T("Добові"));
	grid->SetFieldWidth(field_index, 6);

	field_index = meta_info.getFieldIndexByName("informer_name");
	grid->SetFieldLabel(field_index, _T("Інформатор"));
	grid->SetFieldWidth(field_index, 30);

	field_index = meta_info.getFieldIndexByName("act_no");
	grid->SetFieldLabel(field_index, _T("Тип акту"));
	grid->SetFieldWidth(field_index, 10);

	field_index = meta_info.getFieldIndexByName("id_act");
	grid->SetFieldLabel(field_index, _T("Акт"));
	grid->SetFieldWidth(field_index, 9);

	grid->SetFieldLabel(meta_info.getFieldIndexByName("act_date"), _T("Дата акту"));
	grid->SetFieldLabel(meta_info.getFieldIndexByName("act_reg_date"), _T("Дт. розрах"));

	grid->SetFieldLabel(meta_info.getFieldIndexByName("age"), _T("Вік"));

	field_index = meta_info.getFieldIndexByName("inv");
	grid->SetFieldLabel(field_index, _T("Вади"));
	grid->SetFieldWidth(field_index, 5);

	field_index = meta_info.getFieldIndexByName("lang");
	grid->SetFieldLabel(field_index, _T("Мова"));
	grid->SetFieldWidth(field_index, 5);

	grid->SetFieldLabel(meta_info.getFieldIndexByName("ill"), _T("Хв"));
	grid->SetFieldLabel(meta_info.getFieldIndexByName("zek"), _T("ЗЕК"));

	field_index = meta_info.getFieldIndexByName("appeal_softer");
	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("Апл м"));

	field_index = meta_info.getFieldIndexByName("detect_softer");
	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("Сл м"));

	field_index = meta_info.getFieldIndexByName("reject_appeal");
	grid->SetFieldWidth(field_index, 7);
	grid->SetFieldLabel(field_index, _T("Відм ап"));

	field_index = meta_info.getFieldIndexByName("change_kval_kr");
	grid->SetFieldWidth(field_index, 10);
	grid->SetFieldLabel(field_index, _T("Зм квал кр"));

	field_index = meta_info.getFieldIndexByName("reduce_ep");
	grid->SetFieldWidth(field_index, 8);
	grid->SetFieldLabel(field_index, _T("Зменш еп"));

	field_index = meta_info.getFieldIndexByName("vpr");
	grid->SetFieldLabel(field_index, _T("Випр"));
	grid->SetFieldWidth(field_index, 4);

	field_index = meta_info.getFieldIndexByName("reduce");
	grid->SetFieldLabel(field_index, _T("Зменш"));
	grid->SetFieldWidth(field_index, 6);

	field_index = meta_info.getFieldIndexByName("change_");
	grid->SetFieldLabel(field_index, _T("Змін"));
	grid->SetFieldWidth(field_index, 4);

	field_index = meta_info.getFieldIndexByName("close");
	grid->SetFieldLabel(field_index, _T("Закр"));
	grid->SetFieldWidth(field_index, 4);

	field_index = meta_info.getFieldIndexByName("zv");
	grid->SetFieldLabel(field_index, _T("Звільн"));
	grid->SetFieldWidth(field_index, 6);

	grid->SetFieldLabel(meta_info.getFieldIndexByName("min"), _T("Мін."));

	field_index = meta_info.getFieldIndexByName("nm_suv");
	grid->SetFieldLabel(field_index, _T("Найм. сув."));
	grid->SetFieldWidth(field_index, 10);

	field_index = meta_info.getFieldIndexByName("zv_kr");
	grid->SetFieldLabel(field_index, _T("Звільн. кр."));
	grid->SetFieldWidth(field_index, 10);

	field_index = meta_info.getFieldIndexByName("No_Ch_Ist");
	grid->SetFieldLabel(field_index, _T("Без зм. 1 інст"));
	grid->SetFieldWidth(field_index, 12);

	field_index = meta_info.getFieldIndexByName("change_med");
	grid->SetFieldWidth(field_index, 7);
	grid->SetFieldLabel(field_index, _T("Зм мед"));

	field_index = meta_info.getFieldIndexByName("cancel_med");
	grid->SetFieldWidth(field_index, 8);
	grid->SetFieldLabel(field_index, _T("Скас мед"));

	field_index = meta_info.getFieldIndexByName("Koef");
	grid->SetFieldLabel(field_index, _T("Кзвіт"));
	grid->SetFieldWidth(field_index, 5);

	field_index = meta_info.getFieldIndexByName("user_full_name");
	grid->SetFieldLabel(field_index, _T("Перевірив"));
	grid->SetFieldWidth(field_index, 20);

	grid_as_window = grid;
	createCellWidgetsAndAttachToGrid(grid);

	db_navigator = new CDbNavigator(db_table);
	panel = new CPaymentsNavPanel(conn_, db_table, constraints, \
									stages_list->getMasterResultSet(), \
									informers_list->getMasterResultSet(), \
									checkers_list->getMasterResultSet());
}

void CPaymentsList::initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler) {

	db_table->ConnectDbEventsHandler(evt_handler);
}

std::shared_ptr<CDbTable> CPaymentsList::createDbTable(std::shared_ptr<IDbConnection> conn, \
														const int def_center, \
														const int def_order, \
														CDate def_order_date) {

	std::string query = "SELECT aa.id_center, aa.id_order, aa.order_date,";
	query += " aa.is_paid, aa.id_stage, st.stage_name, aa.cycle, aa.article,";
	query += " aa.fee, aa.outg_post, aa.outg_daynight,";
	query += " aa.id_informer, inf.informer_name, aa.act_no, aa.id_act, aa.act_date, aa.act_reg_date,";
	query += " aa.age,aa.inv,aa.lang,aa.ill,aa.zek,aa.appeal_softer,aa.detect_softer,aa.reject_appeal,";
	query += " aa.vpr, aa.reduce, aa.change_kval_kr, aa.reduce_ep, aa.change_, ";
	query += " aa.close,aa.zv,aa.min,aa.nm_suv,aa.zv_kr,aa.No_Ch_Ist,";
	query += " aa.change_med, aa.cancel_med, aa.Koef, u.user_full_name, aa.id_checker ";
	query += "FROM payments aa INNER JOIN stages st ON aa.id_stage = st.id_st";
	query += " INNER JOIN informers inf ON aa.id_informer = inf.id_inf";
	query += " INNER JOIN users u ON aa.id_checker = u.id_user ";
	query += "WHERE aa.id_center = ? AND aa.id_order = ? AND aa.order_date = ? ";
	query += "ORDER BY id_center, order_date, id_order, cycle, id_stage, act_no";

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
	db_table->markFieldAsPrimaryKey("act_no", "payments");

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
	bool act_date = false, koef = false, act_no = false;
	try {
		is_paid_widget = new CEditableCellWidget(true);
		grid->SetWidgetForFieldByName("is_paid", is_paid_widget);
		is_paid = true;

		cycle_widget = new CIntegerCellWidget(CYCLE_MAX_LEN);
		grid->SetWidgetForFieldByName("cycle", cycle_widget);
		cycle = true;

		act_no_widget = new CComboBoxCellWidget();
		grid->SetWidgetForFieldByName("act_no", act_no_widget);
		act_no = true;

		stages_list = new CDbComboBoxCellWidget(conn, "stage_name", \
											"stages", "payments", db_table);
		stages_list->AddRelation("id_st", "id_stage");
		grid->SetWidgetForFieldByName("stage_name", stages_list);
		stages = true;

		currency_widget = new CCurrencyCellWidget(false);
		grid->SetWidgetForFieldByName("fee", currency_widget);
		fee = true;

		grid->SetWidgetForFieldByName("outg_post", currency_widget);
		grid->SetWidgetForFieldByName("outg_daynight", currency_widget);
		
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
		grid->SetWidgetForFieldByName("appeal_softer", qa_widget);
		grid->SetWidgetForFieldByName("detect_softer", qa_widget);
		grid->SetWidgetForFieldByName("reject_appeal", qa_widget);
		grid->SetWidgetForFieldByName("change_kval_kr", qa_widget);
		grid->SetWidgetForFieldByName("reduce_ep", qa_widget);
		grid->SetWidgetForFieldByName("vpr", qa_widget);
		grid->SetWidgetForFieldByName("reduce", qa_widget);
		grid->SetWidgetForFieldByName("change_", qa_widget);
		grid->SetWidgetForFieldByName("close", qa_widget);
		grid->SetWidgetForFieldByName("zv", qa_widget);
		grid->SetWidgetForFieldByName("min", qa_widget);
		grid->SetWidgetForFieldByName("nm_suv", qa_widget);
		grid->SetWidgetForFieldByName("zv_kr", qa_widget);
		grid->SetWidgetForFieldByName("No_Ch_Ist", qa_widget);
		grid->SetWidgetForFieldByName("change_med", qa_widget);
		grid->SetWidgetForFieldByName("cancel_med", qa_widget);

		koef_widget = new CCurrencyCellWidget(true);
		grid->SetWidgetForFieldByName("Koef", koef_widget);
		koef = true;

		auto chk_stmt = conn->PrepareQuery("SELECT id_user,user_full_name FROM users WHERE is_checker = 1 ORDER BY user_name");
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

		if (!act_no && act_no_widget && !act_no_widget->IsCreated())
			delete act_no_widget;

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

	auto &meta_info = db_table->getQuery().getMetaInfo();

	grid->HideField(meta_info.getFieldIndexByName("id_center"));
	grid->HideField(meta_info.getFieldIndexByName("id_order"));
	grid->HideField(meta_info.getFieldIndexByName("order_date"));
	grid->HideField(meta_info.getFieldIndexByName("id_stage"));
	grid->HideField(meta_info.getFieldIndexByName("id_informer"));
	grid->HideField(meta_info.getFieldIndexByName("id_checker"));

	size_t index = meta_info.getFieldIndexByName("article");
	grid_evt_handler->addAllowedField(grid->GetFieldRelativeIndex(index));
	grid_evt_handler.reset();

	assert(act_no_widget);
	act_no_widget->AddItem(_T("Винагорода"));
	act_no_widget->AddItem(_T("Витрати"));
}

CPaymentsList::~CPaymentsList() {

	if (panel && !cr_panel) delete panel;
	if (db_navigator && !cr_navigator) delete db_navigator;
	if (grid && !cr_grid) delete grid;
}
