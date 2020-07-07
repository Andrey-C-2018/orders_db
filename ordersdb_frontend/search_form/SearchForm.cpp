#include <basic/XConv.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_ext/DependentTableUpdater.h>
#include <xwindows/XLabel.h>
#include <xwindows/XButton.h>
#include <xwindows_ex/HorizontalSizer.h>
#include <xwindows_ex/VerticalSizer.h>
#include <xwindows_ex/XCurrencyField.h>
#include <editable_grid/BooleanCellWidget.h>
#include <editable_grid/CurrencyCellWidget.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <db_controls/FilteringEdit.h>
#include <db_controls/FilteringDbComboBox.h>
#include <db_controls/FilteringDateField.h>
#include <db_controls/BinderControls.h>
#include <db_controls/DbNavigator.h>
#include <db_controls/DbStaticNumField.h>
#include <forms_common/ParametersManager.h>
#include <forms_common/ActNameCellWidget.h>
#include <forms_common/PaymentsDbTableEvtHandler.h>
#include <forms_common/CommonRoutines.h>
#include <forms_common/VernamOneTimePad.h>
#include "SearchForm.h"
#include "SearchFormGridEvtHandler.h"
#include "ZoneFilter.h"
#include "PaidFilter.h"
#include "DoubleBndTarget.h"
#include "OrderingComboBox.h"

class CGridCellWidgetCreator final {
	CDbGrid *grid;
	IGridCellWidget *cell_widget;
	bool allocated_n_attached;
public:
	CGridCellWidgetCreator(CDbGrid *grid_) : grid(grid_), \
					cell_widget(nullptr), allocated_n_attached(false) { }

	template <typename CellWidget, typename ... Args> \
		CellWidget *createAndAttachToGrid(const char *field_name, \
											Args ... args) {

			allocated_n_attached = false;
			this->cell_widget = nullptr;

			CellWidget *cell_widget = new CellWidget(args...);
			this->cell_widget = cell_widget;
			grid->SetWidgetForFieldByName(field_name, cell_widget);
			
			allocated_n_attached = true;
			return cell_widget;
		}

	~CGridCellWidgetCreator() {
	
		if (!allocated_n_attached && cell_widget)
			delete cell_widget;
	}
};

//*****************************************************

constexpr char search_form_version[] = "1.0.2";
const char def_ordering_str[] = "a.id_center_legalaid,a.order_date,a.id,aa.cycle,aa.id_stage";

CSearchForm::CSearchForm(XWindow *parent, const int flags, \
					const Tchar *label, \
					const int X, const int Y, \
					const int width, const int height) : \
				sorting_manager(FIELDS_COUNT), \
				flt_id(nullptr), flt_act(nullptr), flt_order_date_from(nullptr), \
				flt_order_date_to(nullptr), flt_act_reg_date_from(nullptr), \
				flt_act_reg_date_to(nullptr), flt_act_date_from(nullptr), \
				flt_act_date_to(nullptr), flt_payment_date_from(nullptr), \
				flt_payment_date_to(nullptr), flt_center(nullptr), flt_informer(nullptr), \
				flt_order_type(nullptr), flt_stage(nullptr), flt_zone(nullptr), \
				grid(nullptr), advocats_list(nullptr), flt_client(nullptr), \
				centers_list(nullptr), order_types_list(nullptr), stages_list(nullptr), \
				canceling_reasons_list(nullptr), qa_widget(nullptr), \
				grid_x(0), grid_y(0), grid_margin_x(0), grid_margin_y(0), \
				total_fee(nullptr), total_paid(nullptr), total_orders(nullptr), \
				btn_apply_filter(nullptr), btn_add(nullptr), btn_remove(nullptr), \
				btn_rev(nullptr), btn_reset(nullptr), btn_sort(nullptr) {

	props.open("config.ini");
	
	conn = CMySQLConnectionFactory::createConnection(props, vernam_one_time_pad);
	auto version = getFormVersion(conn, "search_form_version");
	if (version != search_form_version)
		throw XException(0, _T("Версія програми не співпадає з версією БД. Використовуйте актуальну версію"));

	CParametersManager::init(&props, conn);
	inserter.evalPermissions();

	setLastChangedUser();

	auto &groups = CParametersManager::getInstance().getGroups();
	bool db_admin = std::find(groups.cbegin(), groups.cend(), "Administrators") != \
								groups.cend();

	auto constraints = std::make_shared<CPaymentsConstraints>();
	constraints->old_stage_locked = !db_admin;
	constraints->wrong_zone = !db_admin;
	constraints->old_order_locked = !db_admin;

	db_table = createDbTable();
	int def_center = CParametersManager::getInstance().getDefaultCenter();
	std::shared_ptr<CPaymentsDbTableEvtHandler> payments_evt_handler = \
						std::make_shared<CPaymentsDbTableEvtHandler>(db_table, \
												def_center, "id_center_legalaid", \
												!db_admin, !db_admin, !db_admin, \
												constraints);
	db_table->ConnectDbEventsHandler(payments_evt_handler);

	createStatisticsStatements();

	grid = new CDbGrid(false, db_table, \
					std::make_shared<CSearchFormGridEvtHandler>(db_table, constraints, \
													std::move(orders_fields_indexes)));
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(db_admin);
	initFilteringControls();

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	displayWidgets();
	payments_evt_handler->calcConstraintsValues();

	adjustUIDependentCellWidgets();
	loadInitialFilterToControls();
	assert(filtering_manager.isFilteringStringChanged());

	Connect(EVT_COMMAND, btn_apply_filter->GetId(), this, &CSearchForm::OnFilterButtonClick);
	Connect(EVT_COMMAND, btn_add->GetId(), this, &CSearchForm::OnAddRecordButtonClick);
	Connect(EVT_COMMAND, btn_remove->GetId(), this, &CSearchForm::OnRemoveButtonClick);
	Connect(EVT_COMMAND, btn_rev->GetId(), this, &CSearchForm::OnRevButtonClick);
	Connect(EVT_COMMAND, btn_reset->GetId(), this, &CSearchForm::OnResetButtonClick);
	Connect(EVT_COMMAND, btn_sort->GetId(), this, &CSearchForm::OnSortButtonClick);
}

void CSearchForm::createStatisticsStatements() {

	std::string query = "SELECT SUM(fee) as total_fee, SUM(fee_parus) as total_paid,";
	query += " COUNT(DISTINCT a.id_center_legalaid, a.id, a.order_date) as orders_count ";
	query += "FROM orders a INNER JOIN payments aa ON a.id_center_legalaid = aa.id_center AND a.id = aa.id_order AND a.order_date = aa.order_date";
	query += " ####";
	query_aggregate.Init(query);

	const auto &params_manager = CParametersManager::getInstance();
	std::string initial_flt = params_manager.getInitialFilteringStr();
	query_aggregate.changeWherePart(\
		ImmutableString<char>(initial_flt.c_str(), initial_flt.size()));
}

void CSearchForm::reloadStatisticsControls() {

	assert(total_fee);
	assert(total_paid);
	assert(total_orders);

	total_fee->Refresh();
	total_paid->Refresh();
	total_orders->Refresh();
}

void CSearchForm::reloadStatisticsControls(std::shared_ptr<IDbStatement> new_stmt) {

	assert(total_fee);
	assert(total_paid);
	assert(total_orders);

	auto new_rs = new_stmt->exec();

	total_fee->Refresh(new_rs);
	total_paid->Refresh(new_rs);
	total_orders->Refresh(std::move(new_rs));
}

void CSearchForm::setFieldsSizes() {

	grid->SetFieldWidth(0, 2);
	grid->SetFieldLabel(0, _T("ЗВ"));
	grid->SetFieldWidth(1, 12);
	grid->SetFieldLabel(1, _T("Центр"));
	grid->SetFieldWidth(2, 17);
	grid->SetFieldLabel(2, _T("Адвокат"));
	grid->SetFieldWidth(3, 5);
	grid->SetFieldLabel(3, _T("№"));
	grid->SetFieldLabel(4, _T("Дата"));
	grid->SetFieldWidth(5, 6);
	grid->SetFieldLabel(5, _T("Тип"));
	grid->SetFieldWidth(6, 30);
	grid->SetFieldLabel(6, _T("Клієнт"));
	grid->SetFieldLabel(7, _T("Дата нар."));
	grid->SetFieldWidth(8, 10);
	grid->SetFieldLabel(8, _T("Етап"));
	grid->SetFieldWidth(9, 17);
	grid->SetFieldLabel(9, _T("Скс./Прп."));
	grid->SetFieldWidth(10, 9);
	grid->SetFieldLabel(10, _T("Наказ"));
	grid->SetFieldLabel(11, _T("Дата скс."));
	grid->SetFieldLabel(12, _T("Сума"));
	grid->SetFieldLabel(13, _T("Витрати"));
	grid->SetFieldLabel(14, _T("Сума 1С"));
	grid->SetFieldWidth(15, 15);
	grid->SetFieldLabel(15, _T("Акт"));
	grid->SetFieldLabel(16, _T("Дт розрах"));
	grid->SetFieldLabel(17, _T("Дт акту"));
	grid->SetFieldLabel(18, _T("Дт р. казн"));
	grid->SetFieldLabel(19, _T("Дата опл."));
	grid->SetFieldWidth(20, 1);
	grid->SetFieldLabel(20, _T("#"));
	grid->SetFieldWidth(21, 30);
	grid->SetFieldLabel(21, _T("Стаття"));
	grid->SetFieldWidth(22, 32);
	grid->SetFieldLabel(22, _T("Інформатор"));

	grid->SetFieldLabel(23, _T("Вік"));
	grid->SetFieldWidth(24, 4);
	grid->SetFieldLabel(24, _T("Вади"));
	grid->SetFieldWidth(25, 4);
	grid->SetFieldLabel(25, _T("Мова"));
	grid->SetFieldLabel(26, _T("Хв"));
	grid->SetFieldLabel(27, _T("ЗЕК"));
	grid->SetFieldWidth(28, 4);
	grid->SetFieldLabel(28, _T("Випр"));
	grid->SetFieldWidth(29, 5);
	grid->SetFieldLabel(29, _T("Зменш"));
	grid->SetFieldWidth(30, 4);
	grid->SetFieldLabel(30, _T("Змін"));
	grid->SetFieldWidth(31, 4);
	grid->SetFieldLabel(31, _T("Закр"));
	grid->SetFieldWidth(32, 5);
	grid->SetFieldLabel(32, _T("Звільн"));
	grid->SetFieldLabel(33, _T("Мін"));
	grid->SetFieldWidth(34, 10);
	grid->SetFieldLabel(34, _T("Найм. сув."));
	grid->SetFieldWidth(35, 11);
	grid->SetFieldLabel(35, _T("Звільн. кр."));
	grid->SetFieldWidth(36, 14);
	grid->SetFieldLabel(36, _T("Без зм. 1 інст"));
	grid->SetFieldWidth(37, 5);
	grid->SetFieldLabel(37, _T("Кзвіт"));
}

void CSearchForm::createCellWidgetsAndAttachToGrid(const bool db_admin) {

	assert(grid);
	assert(!advocats_list);
	assert(!centers_list);
	CGridCellWidgetCreator creator(grid);
	
	auto readonly_widget = creator.createAndAttachToGrid<CEditableCellWidget>(\
										"zone", true);

	auto stmt = conn->PrepareQuery("SELECT id_advocat, adv_name_short, adv_name FROM advocats ORDER BY 2");
	auto result_set = stmt->exec();
	auto rs_metadata = stmt->getResultSetMetadata();
	advocats_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"adv_name_short", \
										conn, 1, result_set, rs_metadata, \
										"orders", db_table);
	advocats_list->AddRelation("id_advocat", "id_adv");

	if (db_admin) {
		centers_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"center_short_name", \
										conn, "center_short_name", "centers", \
										"orders", db_table);
		centers_list->AddRelation("id_center", "id_center_legalaid");
	}
	else
		grid->SetWidgetForFieldByName("center_short_name", readonly_widget);
	
	canceling_reasons_list = new CComboBoxCellWidget();
	grid->SetWidgetForFieldByName("reason", canceling_reasons_list);

	stmt = conn->PrepareQuery("SELECT id_inf, informer_name, informer_full_name FROM informers ORDER BY 2");
	result_set = stmt->exec();
	rs_metadata = stmt->getResultSetMetadata();
	informers_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"informer_name", \
										conn, 1, result_set, rs_metadata, \
										"payments", db_table);
	informers_list->AddRelation("id_inf", "id_informer");

	order_types_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"type_name", \
										conn, "type_name", "order_types", \
										"orders", db_table);
	order_types_list->AddRelation("id_type", "id_order_type");

	stmt = conn->PrepareQuery("SELECT id_st, stage_name FROM stages ORDER BY 2");
	result_set = stmt->exec();
	rs_metadata = stmt->getResultSetMetadata();
	stages_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"stage_name", \
										conn, 1, result_set, rs_metadata, \
										"payments", db_table);
	stages_list->AddRelation("id_st", "id_stage");

	creator.createAndAttachToGrid<CActNameCellWidget>("id_act");

	CCurrencyCellWidget *currency_widget = creator.createAndAttachToGrid<CCurrencyCellWidget>("fee", true);
	grid->SetWidgetForFieldByName("outgoings", currency_widget);

	qa_widget = creator.createAndAttachToGrid<CBooleanCellWidget>("age");
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

	grid->SetWidgetForFieldByName("Koef", currency_widget);
}

void CSearchForm::adjustUIDependentCellWidgets() {

	assert(canceling_reasons_list);

	canceling_reasons_list->AddItem(_T("скасовано"));
	canceling_reasons_list->AddItem(_T("припинено"));
	canceling_reasons_list->AddItem(_T("заміна"));
	canceling_reasons_list->AddItem(_T("відмова"));
	canceling_reasons_list->AddItem(_T("розбіжності"));
	canceling_reasons_list->AddItem(_T("хвороба"));
	canceling_reasons_list->AddItem(_T("розшук"));
	canceling_reasons_list->AddItem(_T("виключений з реєстру"));
	canceling_reasons_list->AddItem(_T("завершено"));
}

void CSearchForm::loadInitialFilterToControls() {

	const auto &params_manager = CParametersManager::getInstance();

	flt_center->SetCurrRecord(params_manager.getDefaultCenter());
	flt_order_date_from->SetLabel(params_manager.getInitialDateW());
	flt_order_date_from->enableIfChanged();
}

void CSearchForm::initFilteringControls() {

	def_binding_target = std::make_shared<CDoubleBndTarget>();

	std::shared_ptr<const IDbResultSet> cn_rs;
	if (centers_list)
		cn_rs = centers_list->getMasterResultSet();
	else
		cn_rs = conn->ExecQuery("SELECT * FROM centers ORDER BY id_center");

	flt_center = new CFilteringDbComboBox(filtering_manager, cn_rs, 2, 0);
	auto combo_binder = std::make_shared<CDbComboBoxBinderControl>(flt_center);
	int id_expr = filtering_manager.addExpr("a.id_center_legalaid = ?", combo_binder);
	flt_center->setExprId(id_expr);

	flt_id = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> id_binder = std::make_shared<CIntWidgetBinderControl>(flt_id);
	ImmutableString<char> expr("a.id = ?", sizeof("a.id = ?") - 1);
	id_expr = filtering_manager.addExpr(expr, id_binder);
	flt_id->setExprId(id_expr);

	flt_order_date_from = new CFilteringDateField(filtering_manager, this);
	auto date_binder = std::make_shared<CDateWidgetBinderControl>(flt_order_date_from);
	id_expr = filtering_manager.addExpr("a.order_date >= ?", date_binder);
	flt_order_date_from->setExprId(id_expr);

	flt_order_date_to = new CFilteringDateField(filtering_manager, this);
	date_binder = std::make_shared<CDateWidgetBinderControl>(flt_order_date_to);
	id_expr = filtering_manager.addExpr("a.order_date <= ?", date_binder);
	flt_order_date_to->setExprId(id_expr);

	flt_order_type = new CFilteringDbComboBox(filtering_manager, \
										order_types_list->getMasterResultSet(), 2, 0);
	combo_binder = std::make_shared<CDbComboBoxBinderControl>(flt_order_type);
	id_expr = filtering_manager.addExpr("a.id_order_type = ?", combo_binder);
	flt_order_type->setExprId(id_expr);

	flt_advocat = new CFilteringDbComboBox(filtering_manager, \
											advocats_list->getMasterResultSet(), 2, 0);
	combo_binder = std::make_shared<CDbComboBoxBinderControl>(flt_advocat);
	id_expr = filtering_manager.addExpr("a.id_adv = ?", combo_binder);
	flt_advocat->setExprId(id_expr);

	flt_client = new CFilteringEdit(filtering_manager, this);
	auto str_binder = std::make_shared<CStringWidgetBinderControl>(flt_client);
	id_expr = filtering_manager.addExpr("a.client_name LIKE ?", str_binder);
	flt_client->setExprId(id_expr);

	flt_zone = new CZoneFilter(filtering_manager);

	flt_act = new CFilteringEdit(filtering_manager, this);
	str_binder = std::make_shared<CStringWidgetBinderControl>(flt_act);
	id_expr = filtering_manager.addExpr("aa.id_act LIKE ?", str_binder);
	flt_act->setExprId(id_expr);

	flt_informer = new CFilteringDbComboBox(filtering_manager, \
											informers_list->getMasterResultSet(), 2, 0);
	combo_binder = std::make_shared<CDbComboBoxBinderControl>(flt_informer);
	id_expr = filtering_manager.addExpr("aa.id_informer = ?", combo_binder);
	flt_informer->setExprId(id_expr);

	flt_stage = new CFilteringDbComboBox(filtering_manager, \
											stages_list->getMasterResultSet(), 1, 0);
	combo_binder = std::make_shared<CDbComboBoxBinderControl>(flt_stage);
	id_expr = filtering_manager.addExpr("aa.id_stage = ?", combo_binder);
	flt_stage->setExprId(id_expr);

	flt_act_reg_date_from = new CFilteringDateField(filtering_manager, this);
	date_binder = std::make_shared<CDateWidgetBinderControl>(flt_act_reg_date_from);
	id_expr = filtering_manager.addExpr("aa.act_reg_date >= ?", date_binder);
	flt_act_reg_date_from->setExprId(id_expr);

	flt_act_reg_date_to = new CFilteringDateField(filtering_manager, this);
	date_binder = std::make_shared<CDateWidgetBinderControl>(flt_act_reg_date_to);
	id_expr = filtering_manager.addExpr("aa.act_reg_date <= ?", date_binder);
	flt_act_reg_date_to->setExprId(id_expr);

	flt_act_date_from = new CFilteringDateField(filtering_manager, this);
	date_binder = std::make_shared<CDateWidgetBinderControl>(flt_act_date_from);
	id_expr = filtering_manager.addExpr("aa.act_date >= ?", date_binder);
	flt_act_date_from->setExprId(id_expr);

	flt_act_date_to = new CFilteringDateField(filtering_manager, this);
	date_binder = std::make_shared<CDateWidgetBinderControl>(flt_act_date_to);
	id_expr = filtering_manager.addExpr("aa.act_date <= ?", date_binder);
	flt_act_date_to->setExprId(id_expr);

	flt_payment_date_from = new CFilteringDateField(filtering_manager, this);
	date_binder = std::make_shared<CDateWidgetBinderControl>(flt_payment_date_from);
	id_expr = filtering_manager.addExpr("aa.payment_date >= ?", date_binder);
	flt_payment_date_from->setExprId(id_expr);

	flt_payment_date_to = new CFilteringDateField(filtering_manager, this);
	date_binder = std::make_shared<CDateWidgetBinderControl>(flt_payment_date_to);
	id_expr = filtering_manager.addExpr("aa.payment_date <= ?", date_binder);
	flt_payment_date_to->setExprId(id_expr);

	flt_paid = new CPaidFilter(filtering_manager);
}

std::shared_ptr<CDbTable> CSearchForm::createDbTable() {

	orders_fields_indexes.insert({0, 1, 2, 3, 4, 5, 6, 7, 9, 10});

	std::string query = "SELECT a.zone, c.center_short_name, b.adv_name_short, a.id, a.order_date,";
	query += " t.type_name, a.client_name, a.bdate, sta.stage_name, a.reason, a.cancel_order, a.cancel_date, aa.fee, aa.outgoings, aa.fee_parus,";
	query += " aa.id_act, aa.act_reg_date, aa.act_date, aa.bank_reg_date, aa.payment_date,";
	query += " aa.cycle, aa.article, inf.informer_name,";
	query += " aa.age,aa.inv,aa.lang,aa.ill,aa.zek,aa.vpr,aa.reduce,aa.change_,";
	query += " aa.close,aa.zv,aa.min,aa.nm_suv,aa.zv_kr,aa.No_Ch_Ist,aa.Koef,";
	query += " aa.id_stage, a.id_center_legalaid, a.id_adv, a.id_order_type, aa.id_informer ";
	query += "FROM orders a INNER JOIN payments aa ON a.id_center_legalaid = aa.id_center AND a.id = aa.id_order AND a.order_date = aa.order_date";
	query += " INNER JOIN advocats b ON a.id_adv = b.id_advocat";
	query += " INNER JOIN order_types t ON a.id_order_type = t.id_type";
	query += " INNER JOIN informers inf ON aa.id_informer = inf.id_inf";
	query += " INNER JOIN centers c ON a.id_center_legalaid = c.id_center";
	query += " INNER JOIN stages sta ON aa.id_stage = sta.id_st";
	query += " ####";
	query += " ORDER BY ";
	query += def_ordering_str;
	query_modifier.Init(query);

	const auto &params_manager = CParametersManager::getInstance();
	std::string initial_flt = params_manager.getInitialFilteringStr();
	query_modifier.changeWherePart(\
		ImmutableString<char>(initial_flt.c_str(), initial_flt.size()));

	auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt), true);
	db_table->setPrimaryTableForQuery("orders");
	db_table->markFieldAsPrimaryKey("id_center_legalaid", "orders");
	db_table->markFieldAsPrimaryKey("id", "orders");
	db_table->markFieldAsPrimaryKey("order_date", "orders");

	db_table->addPrimaryKeyAsRef("id_center", "payments", \
									"id_center_legalaid", "orders");
	db_table->addPrimaryKeyAsRef("id_order", "payments", \
									"id", "orders");
	db_table->addPrimaryKeyAsRef("order_date", "payments", \
									"order_date", "orders");
	db_table->markFieldAsPrimaryKey("id_stage", "payments");
	db_table->markFieldAsPrimaryKey("cycle", "payments");
	
	return db_table;
}

void CSearchForm::displayWidgets() {

	XRect rc;
	const int edit_flags = FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
		FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL;

	GetClientRect(rc);
	CVerticalSizer main_sizer(this, 0, 0, rc.right, rc.bottom, \
								10, 10, 10, 10, \
								DEF_GUI_VERT_GAP, DEF_GUI_ROW_HEIGHT);

	CHorizontalSizer sizer(CSizerPreferences(0, 0, 0, 0, DEF_GUI_HORZ_GAP));
	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Адвокат: "), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_advocat, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(250, DEF_GUI_ROW_HEIGHT), 250);
		flt_advocat->setTabStopManager(this);
		inserter.getOrdersInserter().SetAdvocatWidget(flt_advocat);

		sizer.addWidget(new XLabel(), _T("Центр: "), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_center, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(180, DEF_GUI_ROW_HEIGHT), 200);
		flt_center->setTabStopManager(this);
		inserter.SetCenterBox(flt_center);

		sizer.addWidget(new XLabel(), _T("Інформатор: "), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_informer, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(250, DEF_GUI_ROW_HEIGHT), 250);
		flt_informer->setTabStopManager(this);
		inserter.getPaymentsInserter().setInformerWidget(flt_informer);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("№ дор.: "), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
		sizer.addWidget(flt_id, _T(""), FL_WINDOW_VISIBLE, \
						XSize(45, DEF_GUI_ROW_HEIGHT));
		inserter.SetIdOrderWidget(flt_id);

		sizer.addWidget(new XLabel(), _T("Тип: "), FL_WINDOW_VISIBLE, \
						XSize(35, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_order_type, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(100, DEF_GUI_ROW_HEIGHT), 250);
		flt_order_type->setTabStopManager(this);
		inserter.getOrdersInserter().SetOrderTypeWidget(flt_order_type);

		sizer.addWidget(new XLabel(), _T("Дата з: "), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_order_date_from, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.SetOrderDateWidget(flt_order_date_from);

		sizer.addWidget(new XLabel(), _T("Дата до: "), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_order_date_to, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Опис:"), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *article = new XTabStopEdit(this);
		sizer.addWidget(article, _T(""), edit_flags, XSize(320, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setArticleWidget(article);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("№ акта: "), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
		sizer.addWidget(flt_act, _T(""), FL_WINDOW_VISIBLE, \
						XSize(90, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setActWidget(flt_act);

		sizer.addWidget(new XLabel(), _T("Етап: "), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_stage, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(110, DEF_GUI_ROW_HEIGHT), 250);
		flt_stage->setTabStopManager(this);
		inserter.getPaymentsInserter().setStageWidget(flt_stage);

		sizer.addWidget(new XLabel(), _T("№ розгляду:"), FL_WINDOW_VISIBLE, \
						XSize(90, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *cycle = new XTabStopEdit(this);
		sizer.addWidget(cycle, _T(""), edit_flags, XSize(30, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setCycleWidget(cycle);

		sizer.addWidget(new XLabel(), _T("Сума: "), FL_WINDOW_VISIBLE, \
						XSize(45, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *fee = new XCurrencyField(this);
		sizer.addWidget(fee, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(90, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setFeeWidget(fee);

		sizer.addWidget(new XLabel(), _T("Витрати: "), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *outgoings = new XCurrencyField(this);
		sizer.addWidget(outgoings, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(90, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setOutgoingsWidget(outgoings);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Дата розрах. з: "), FL_WINDOW_VISIBLE, \
						XSize(70, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_reg_date_from, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setActRegDateWidget(flt_act_reg_date_from);

		sizer.addWidget(new XLabel(), _T("Дата розрах. до: "), FL_WINDOW_VISIBLE, \
						XSize(80, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_reg_date_to, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
	
		sizer.addWidget(new XLabel(), _T("Дата акту з: "), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_date_from, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setActDateWidget(flt_act_date_from);

		sizer.addWidget(new XLabel(), _T("Дата акту до: "), FL_WINDOW_VISIBLE, \
						XSize(65, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_date_to, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Дата оплати з: "), FL_WINDOW_VISIBLE, \
						XSize(70, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_payment_date_from, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInserter().setPaymentDateWidget(flt_payment_date_from);

		sizer.addWidget(new XLabel(), _T("Дата оплати до: "), FL_WINDOW_VISIBLE, \
						XSize(75, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_payment_date_to, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Клієнт:"), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
		sizer.addWidget(flt_client, _T(""), edit_flags, XSize(320, DEF_GUI_ROW_HEIGHT));
		inserter.getOrdersInserter().SetClientWidget(flt_client);

		sizer.addWidget(new XLabel(), _T("Дата народж:"), FL_WINDOW_VISIBLE, \
						XSize(55, DEF_GUI_ROW_HEIGHT + 10));
		XDateField *bdate = new XDateField(this);
		sizer.addWidget(bdate, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getOrdersInserter().SetClientBirthDateWidget(bdate);

		sizer.addWidget(new XLabel(), _T("Зона відп.:"), FL_WINDOW_VISIBLE, \
						XSize(85, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addResizeableWidget(flt_zone, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(50, DEF_GUI_ROW_HEIGHT), 100);
		
		sizer.addWidget(new XLabel(), _T("Оплачено:"), FL_WINDOW_VISIBLE, \
						XSize(85, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addResizeableWidget(flt_paid, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(50, DEF_GUI_ROW_HEIGHT), 100);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		btn_apply_filter = new XButton();
		sizer.addWidget(btn_apply_filter, _T("Фільтр"), FL_WINDOW_VISIBLE, \
						XSize(70, DEF_GUI_ROW_HEIGHT));

		btn_reset = new XButton();
		sizer.addWidget(btn_reset, _T("Скд"), FL_WINDOW_VISIBLE, \
						XSize(35, DEF_GUI_ROW_HEIGHT));
		
		auto db_navigator = new CDbNavigator(db_table);
		sizer.addWidget(db_navigator, _T(""), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));

		btn_add = new XButton();
		sizer.addWidget(btn_add, _T("+"), FL_WINDOW_VISIBLE, \
						XSize(30, DEF_GUI_ROW_HEIGHT));

		const auto &params_manager = CParametersManager::getInstance();
		int rm_button_flags = FL_WINDOW_VISIBLE * (params_manager.getIdUser() == 1);
		btn_remove = new XButton();
		sizer.addWidget(btn_remove, _T("-"), rm_button_flags, \
						XSize(30, DEF_GUI_ROW_HEIGHT));

		auto stmt_aggregate = conn->PrepareQuery(query_aggregate.getQuery().c_str());
		auto rs_aggr = stmt_aggregate->exec();
		sizer.addWidget(new XLabel(), _T("Заг. сума:"), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT + 10));
		total_fee = new CDbStaticDecimalField(rs_aggr, 0);
		sizer.addWidget(total_fee, _T(""), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Сума 1С:"), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT + 10));
		total_paid = new CDbStaticDecimalField(rs_aggr, 1);
		sizer.addWidget(total_paid, _T(""), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("К-сть дор.:"), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT + 10));
		total_orders = new CDbStaticIntField(rs_aggr, 2);
		sizer.addWidget(total_orders, _T(""), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));

		btn_rev = new XButton();
		sizer.addWidget(btn_rev, _T("↑"), FL_WINDOW_VISIBLE, \
						XSize(20, DEF_GUI_ROW_HEIGHT));

		auto combo_sorting = new COrderingComboBox(&sorting_manager);
		sizer.addResizeableWidget(combo_sorting, _T(""), \
						FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | FL_COMBOBOX_DROPDOWN, \
						XSize(120, DEF_GUI_ROW_HEIGHT), 150);
		initOrdering(combo_sorting);

		btn_sort = new XButton();
		sizer.addWidget(btn_sort, _T("Сорт"), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
				
	main_sizer.popNestedSizer();

	XRect grid_coords = main_sizer.addLastWidget(grid);

	grid->HideField(38);
	grid->HideField(39);
	grid->HideField(40);
	grid->HideField(41);
	grid->HideField(42);
	grid->SetFocus();

	grid_x = grid_coords.left;
	grid_y = grid_coords.top;

	XRect margins = main_sizer.getMargins();
	grid_margin_x = margins.left;
	grid_margin_y = margins.top;

	inserter.prepare(conn);
}

void CSearchForm::initOrdering(COrderingComboBox *ordering_box) {

	assert(ordering_box);
	const auto &meta_info = db_table->getQuery().getMetaInfo();
	ordering_box->addOrderingField("zone", meta_info, grid);
	ordering_box->addOrderingField("center_short_name", meta_info, grid);
	ordering_box->addOrderingField("adv_name_short", meta_info, grid);
	ordering_box->addOrderingField("order_date", meta_info, grid);
	ordering_box->addOrderingField("id", meta_info, grid);
	ordering_box->addOrderingField("type_name", meta_info, grid);
	ordering_box->addOrderingField("client_name", meta_info, grid);
	ordering_box->addOrderingField("stage_name", meta_info, grid);
	ordering_box->addOrderingField("reason", meta_info, grid);
	ordering_box->addOrderingField("cancel_date", meta_info, grid);
	ordering_box->addOrderingField("act_reg_date", meta_info, grid);
	ordering_box->addOrderingField("act_date", meta_info, grid);
	ordering_box->addOrderingField("payment_date", meta_info, grid);
	ordering_box->addOrderingField("informer_name", meta_info, grid);
}

void CSearchForm::OnSortButtonClick(XCommandEvent *eve) {

	if (sorting_manager.isOrderingChanged()) {
		
		if (sorting_manager.empty()) {
			ImmutableString<char> p_str(def_ordering_str, \
										sizeof(def_ordering_str) - 1);
			query_modifier.changeOrdering(p_str);
		}
		else
			query_modifier.changeOrdering(sorting_manager.buildOrderingString());
		auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());
		
		def_binding_target->replaceFirst(stmt);
		filtering_manager.applyForced(stmt);
		db_table->reload(stmt);
	}
}

void CSearchForm::OnFilterButtonClick(XCommandEvent *eve) {

	if (filtering_manager.isFilteringStringChanged()) {

		auto new_flt_str = filtering_manager.buildFilteringString();
		query_modifier.changeWherePart(new_flt_str);
		query_aggregate.changeWherePart(new_flt_str);

		auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());
		auto stmt_aggregate = conn->PrepareQuery(query_aggregate.getQuery().c_str());
		def_binding_target->replaceFirst(stmt);
		def_binding_target->replaceSecond(stmt_aggregate);

		if (filtering_manager.apply(def_binding_target)) {
			db_table->reload(stmt);
			reloadStatisticsControls(stmt_aggregate);
		}
	}
	else {

		if (filtering_manager.apply(def_binding_target)) {
			db_table->reload();
			reloadStatisticsControls();
		}
	}
}

void CSearchForm::OnAddRecordButtonClick(XCommandEvent *eve) {

	if (inserter.insert()) {
		db_table->reload();
		reloadStatisticsControls();
	}
}

void CSearchForm::OnRemoveButtonClick(XCommandEvent *eve) {

	int option = _plMessageBoxYesNo(_T("Видалити поточний запис?"));
	if (option == IDYES) {
		db_table->removeCurrentRecord();
		reloadStatisticsControls();
	}
}

void CSearchForm::OnRevButtonClick(XCommandEvent *eve) {

	bool s_order = db_table->invertSortingOrder();
	if (s_order)
		btn_rev->SetLabel(_T("↑"));
	else
		btn_rev->SetLabel(_T("↓"));
}

void CSearchForm::OnResetButtonClick(XCommandEvent *eve) {

	filtering_manager.disableAll();
	loadInitialFilterToControls();
	OnFilterButtonClick(eve);
}

void CSearchForm::OnSize(XSizeEvent *eve) {

	int width = eve->GetWidth();
	int height = eve->GetHeight();

	if (!(grid && width && height)) return;

	grid->MoveWindow(grid_x, grid_y, \
		width - 2 * grid_margin_x, \
		height - grid_y - grid_margin_y);
}

CSearchForm::~CSearchForm() {

	if (grid && !grid->IsCreated()) delete grid;

	if (flt_zone && !flt_zone->IsCreated()) delete flt_zone;
	if (flt_paid && !flt_paid->IsCreated()) delete flt_paid;
	if (flt_order_type && !flt_order_type->IsCreated()) delete flt_order_type;
	if (flt_stage && !flt_stage->IsCreated()) delete flt_stage;
	if (flt_center && !flt_center->IsCreated()) delete flt_center;
	if (flt_client && !flt_client->IsCreated()) delete flt_client;
	if (flt_informer && !flt_informer->IsCreated()) delete flt_informer;
	if (flt_advocat && !flt_advocat->IsCreated()) delete flt_advocat;
	if (flt_payment_date_from && !flt_payment_date_from->IsCreated()) delete flt_payment_date_from;
	if (flt_payment_date_to && !flt_payment_date_to->IsCreated()) delete flt_payment_date_to;
	if (flt_act_date_to && !flt_act_date_to->IsCreated()) delete flt_act_date_to;
	if (flt_act_date_from && !flt_act_date_from->IsCreated()) delete flt_act_date_from;
	if (flt_act_reg_date_to && !flt_act_reg_date_to->IsCreated()) delete flt_act_reg_date_to;
	if (flt_act_reg_date_from && !flt_act_reg_date_from->IsCreated()) delete flt_act_reg_date_from;
	if (flt_order_date_to && !flt_order_date_to->IsCreated()) delete flt_order_date_to;
	if (flt_order_date_from && !flt_order_date_from->IsCreated()) delete flt_order_date_from;
	if (flt_act && !flt_act->IsCreated()) delete flt_act;
	if (flt_id && !flt_id->IsCreated()) delete flt_id;
}
