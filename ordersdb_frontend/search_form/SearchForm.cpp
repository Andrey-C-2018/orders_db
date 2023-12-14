#include <basic/XConv.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_ext/DependentTableUpdater.h>
#include <db_ext/PaginalStatement.h>
#include <xwindows/XLabel.h>
#include <xwindows/XButton.h>
#include <xwindows_ex/HorizontalSizer.h>
#include <xwindows_ex/VerticalSizer.h>
#include <xwindows_ex/XCurrencyField.h>
#include <editable_grid/BooleanCellWidget.h>
#include <editable_grid/CurrencyCellWidget.h>
#include <editable_grid/DateCellWidget.h>
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
#include <forms_common/ActDateCellWidget.h>
#include <forms_common/PaymentsDbTableEvtHandler.h>
#include <forms_common/CommonRoutines.h>
#include <forms_common/VernamOneTimePad.h>
#include <forms_common/ActNoFilter.h>
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
	explicit CGridCellWidgetCreator(CDbGrid *grid_) : grid(grid_), \
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

constexpr char search_form_version[] = "1.1.2";
const char def_ordering_str[] = "a.id_center_legalaid DESC,a.order_date DESC,a.id DESC,aa.cycle DESC,aa.id_stage DESC,aa.act_no";
constexpr size_t HIDDEN_FIELDS_COUNT = 7; // hidden field must be at the end of the field list

CSearchForm::CSearchForm(XWindow *parent, const int flags, \
					const Tchar *label, \
					const int X, const int Y, \
					const int width, const int height) : \
				sorting_manager(FieldsList::FIELDS_COUNT), uploader(HIDDEN_FIELDS_COUNT), \
				flt_id(nullptr), flt_act(nullptr), flt_order_date_from(nullptr), \
				flt_order_date_to(nullptr), flt_defender(nullptr), \
                flt_act_reg_date_from(nullptr), flt_act_reg_date_to(nullptr), \
                flt_act_date_from(nullptr), flt_act_date_to(nullptr), \
                flt_payment_date_from(nullptr), flt_payment_date_to(nullptr), \
                flt_paid(nullptr), flt_center(nullptr), flt_informer(nullptr), \
				flt_order_type(nullptr), flt_stage(nullptr), flt_zone(nullptr), \
				flt_act_no(nullptr), \
				grid(nullptr), defenders_list(nullptr), flt_client(nullptr), \
				centers_list(nullptr), order_types_list(nullptr), stages_list(nullptr), \
				checkers_list(nullptr), canceling_reasons_list(nullptr), act_no_list(nullptr), \
				informers_list(nullptr), qa_widget(nullptr), \
				grid_x(0), grid_y(0), grid_margin_x(0), grid_margin_y(0), \
				total_fee(nullptr), total_paid(nullptr), total_orders(nullptr), \
				btn_apply_filter(nullptr), btn_add(nullptr), btn_remove(nullptr), \
				btn_rev(nullptr), btn_reset(nullptr), btn_sort(nullptr), btn_upload(nullptr) {

	props.open();
	fields_list.open(props);
	
	conn = CMySQLConnectionFactory::createConnection(props, vernam_one_time_pad);
	auto version = getFormVersion(conn, "search_form_version");
	if (version != search_form_version)
		throw XException(0, _T("Версія програми не співпадає з версією БД. Використовуйте актуальну версію"));

	CParametersManager::init(&props, conn);
	inserter.evalPermissions();

	setLastChangedUser();

	auto &perm_mgr = CParametersManager::getInstance().getPermissions();
	bool db_admin = perm_mgr.isAdmin();
	bool db_local_admin = perm_mgr.isLocalAdmin();

	constraints = std::make_shared<CPaymentsConstraints>();
	constraints->old_stage_locked = !db_admin && !db_local_admin;
	constraints->stage_finalized = !db_admin && !db_local_admin;
	constraints->wrong_zone = !db_admin;
	constraints->old_order_locked = !db_admin && !db_local_admin;
	
	db_table = createDbTable();
	int def_center = CParametersManager::getInstance().getDefaultCenter();
	std::shared_ptr<CPaymentsDbTableEvtHandler> payments_evt_handler = \
						std::make_shared<CPaymentsDbTableEvtHandler>(db_table, \
												def_center, "id_center_legalaid", \
												constraints->old_stage_locked, \
												constraints->wrong_zone, \
												constraints->old_order_locked, \
												constraints->stage_finalized, \
												constraints);
	db_table->ConnectDbEventsHandler(payments_evt_handler);
	
	createStatisticsStatements();

	createDbGrid(constraints);
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(db_admin);
	initFilteringControls();

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	displayWidgets();
	uploader.init(this->GetInternalHandle(), db_table, &props);
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
	Connect(EVT_COMMAND, btn_upload->GetId(), this, &CSearchForm::OnUploadButtonClick);
}

void CSearchForm::createDbGrid(std::shared_ptr<CPaymentsConstraints> constraints) {

	const auto &meta_info = db_table->getMetaInfo();
	// allowed stage fields if the order is locked
	size_t allowed_indexes[] = { meta_info.getFieldIndexByName("reason"), \
								meta_info.getFieldIndexByName("cancel_order"), \
								meta_info.getFieldIndexByName("cancel_date"), \
								meta_info.getFieldIndexByName("article"), \
								meta_info.getFieldIndexByName("fee_parus"), \
								meta_info.getFieldIndexByName("bank_reg_date"), \
								meta_info.getFieldIndexByName("payment_date") };
		
	auto orders_indexes = meta_info.getAllTableFieldsIndexes("orders");
	//orders_indexes.erase(orders_indexes.end() - 3, orders_indexes.end());
	orders_indexes.push_back(meta_info.getFieldIndexByName("center_short_name"));
	orders_indexes.push_back(meta_info.getFieldIndexByName("name_short")); // defender_name_short
	orders_indexes.push_back(meta_info.getFieldIndexByName("type_name"));
	
	grid = new CDbGrid(false, db_table, \
		std::make_shared<CSearchFormGridEvtHandler>(db_table, constraints, \
												orders_indexes, allowed_indexes));
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
	
	const auto &meta_info = db_table->getMetaInfo();
	size_t field_index = meta_info.getFieldIndexByName("zone");
	grid->SetFieldWidth(field_index, 2);
	grid->SetFieldLabel(field_index, _T("ЗВ"));

	field_index = meta_info.getFieldIndexByName("center_short_name");
	grid->SetFieldWidth(field_index, 12);
	grid->SetFieldLabel(field_index, _T("Центр"));

	field_index = meta_info.getFieldIndexByName("name_short");
	grid->SetFieldWidth(field_index, 17);
	grid->SetFieldLabel(field_index, _T("Адвокат"));

	field_index = meta_info.getFieldIndexByName("id");
	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("№"));

	grid->SetFieldLabel(meta_info.getFieldIndexByName("order_date", "orders"), _T("Дата"));

	field_index = meta_info.getFieldIndexByName("type_name");
	grid->SetFieldWidth(field_index, 6);
	grid->SetFieldLabel(field_index, _T("Тип"));

	field_index = meta_info.getFieldIndexByName("client_name");
	grid->SetFieldWidth(field_index, 30);
	grid->SetFieldLabel(field_index, _T("Клієнт"));

	grid->SetFieldLabel(meta_info.getFieldIndexByName("bdate"), _T("Дата нар."));

	field_index = meta_info.getFieldIndexByName("stage_name");
	grid->SetFieldWidth(field_index, 10);
	grid->SetFieldLabel(field_index, _T("Етап"));

	field_index = meta_info.getFieldIndexByName("reason");
	grid->SetFieldWidth(field_index, 17);
	grid->SetFieldLabel(field_index, _T("Скс./Прп."));

	field_index = meta_info.getFieldIndexByName("cancel_order");
	grid->SetFieldWidth(field_index, 9);
	grid->SetFieldLabel(field_index, _T("Наказ"));

	grid->SetFieldLabel(meta_info.getFieldIndexByName("cancel_date"), _T("Дата скс."));
	grid->SetFieldLabel(meta_info.getFieldIndexByName("fee"), _T("Сума"));
	grid->SetFieldLabel(meta_info.getFieldIndexByName("fee_parus"), _T("Сума 1С"));

	field_index = meta_info.getFieldIndexByName("outg_post");
	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("Пошт."));

	field_index = meta_info.getFieldIndexByName("outg_daynight");
	grid->SetFieldWidth(field_index, 6);
	grid->SetFieldLabel(field_index, _T("Добові"));
	
	field_index = meta_info.getFieldIndexByName("act_no");
	grid->SetFieldWidth(field_index, 10);
	grid->SetFieldLabel(field_index, _T("Тип акту"));

	field_index = meta_info.getFieldIndexByName("id_act");
	grid->SetFieldWidth(field_index, 15);
	grid->SetFieldLabel(field_index, _T("Акт"));

	grid->SetFieldLabel(meta_info.getFieldIndexByName("act_reg_date"), _T("Дт розрах"));
	grid->SetFieldLabel(meta_info.getFieldIndexByName("act_date"), _T("Дт акту"));
	grid->SetFieldLabel(meta_info.getFieldIndexByName("bank_reg_date"), _T("Дт р. казн"));
	grid->SetFieldLabel(meta_info.getFieldIndexByName("payment_date"), _T("Дата опл."));

	field_index = meta_info.getFieldIndexByName("cycle");
	grid->SetFieldWidth(field_index, 1);
	grid->SetFieldLabel(field_index, _T("#"));

	field_index = meta_info.getFieldIndexByName("article");
	grid->SetFieldWidth(field_index, 40);
	grid->SetFieldLabel(field_index, _T("Стаття"));

	field_index = meta_info.getFieldIndexByName("informer_name");
	grid->SetFieldWidth(field_index, 32);
	grid->SetFieldLabel(field_index, _T("Інформатор"));

	grid->SetFieldLabel(meta_info.getFieldIndexByName("age"), _T("Вік"));

	field_index = meta_info.getFieldIndexByName("inv");
	grid->SetFieldWidth(field_index, 4);
	grid->SetFieldLabel(field_index, _T("Вади"));

	field_index = meta_info.getFieldIndexByName("lang");
	grid->SetFieldWidth(field_index, 4);
	grid->SetFieldLabel(field_index, _T("Мова"));

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
	grid->SetFieldWidth(field_index, 4);
	grid->SetFieldLabel(field_index, _T("Випр"));

	field_index = meta_info.getFieldIndexByName("reduce");
	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("Зменш"));

	field_index = meta_info.getFieldIndexByName("change_");
	grid->SetFieldWidth(field_index, 4);
	grid->SetFieldLabel(field_index, _T("Змін"));

	field_index = meta_info.getFieldIndexByName("close");
	grid->SetFieldWidth(field_index, 4);
	grid->SetFieldLabel(field_index, _T("Закр"));

	field_index = meta_info.getFieldIndexByName("zv");
	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("Звільн"));

	grid->SetFieldLabel(meta_info.getFieldIndexByName("min"), _T("Мін"));

	field_index = meta_info.getFieldIndexByName("nm_suv");
	grid->SetFieldWidth(field_index, 10);
	grid->SetFieldLabel(field_index, _T("Найм. сув."));

	field_index = meta_info.getFieldIndexByName("zv_kr");
	grid->SetFieldWidth(field_index, 11);
	grid->SetFieldLabel(field_index, _T("Звільн. кр."));

	field_index = meta_info.getFieldIndexByName("No_Ch_Ist");
	grid->SetFieldWidth(field_index, 14);
	grid->SetFieldLabel(field_index, _T("Без зм. 1 інст"));

	field_index = meta_info.getFieldIndexByName("change_med");
	grid->SetFieldWidth(field_index, 7);
	grid->SetFieldLabel(field_index, _T("Зм мед"));

	field_index = meta_info.getFieldIndexByName("cancel_med");
	grid->SetFieldWidth(field_index, 8);
	grid->SetFieldLabel(field_index, _T("Скас мед"));

	field_index = meta_info.getFieldIndexByName("Koef");
	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("Кзвіт"));

	field_index = meta_info.getFieldIndexByName("user_full_name");
	grid->SetFieldWidth(field_index, 30);
	grid->SetFieldLabel(field_index, _T("Перевірив"));

	field_index = meta_info.getFieldIndexByName("rejection_date");
	grid->SetFieldWidth(field_index, 15);
	grid->SetFieldLabel(field_index, _T("Дт пов на доопр"));

	field_index = meta_info.getFieldIndexByName("rejection_reason");
	grid->SetFieldWidth(field_index, 30);
	grid->SetFieldLabel(field_index, _T("Причина доопр"));
}

void CSearchForm::createCellWidgetsAndAttachToGrid(const bool db_admin) {

	assert(grid);
	assert(!defenders_list);
	assert(!centers_list);
	CGridCellWidgetCreator creator(grid);
	
	auto readonly_widget = creator.createAndAttachToGrid<CEditableCellWidget>(\
										"zone", true);
	creator.createAndAttachToGrid<CDateCellWidget>("order_date", false);
		
	auto stmt = conn->PrepareQuery("SELECT id_person, name_short, name FROM people ORDER BY 2");
	auto result_set = stmt->exec();
	auto rs_metadata = stmt->getResultSetMetadata();
	defenders_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"name_short", \
										conn, 1, result_set, rs_metadata, \
										"orders", db_table);
	defenders_list->AddRelation("id_person", "id_defender");

	if (db_admin) {
		centers_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"center_short_name", \
										conn, "center_short_name", "centers", \
										"orders", db_table);
		centers_list->AddRelation("id_center", "id_center_legalaid");
	}
	else
		grid->SetWidgetForFieldByName("center_short_name", readonly_widget);
	
	canceling_reasons_list = new CComboBoxCellWidget(true);
	grid->SetWidgetForFieldByName("reason", canceling_reasons_list);

	stmt = conn->PrepareQuery("SELECT id_inf, informer_name FROM informers ORDER BY 2");
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

	act_no_list = new CComboBoxCellWidget();
	grid->SetWidgetForFieldByName("act_no", act_no_list);

	creator.createAndAttachToGrid<CActNameCellWidget>("id_act");

	CCurrencyCellWidget *currency_widget = creator.createAndAttachToGrid<CCurrencyCellWidget>("fee", true);
	grid->SetWidgetForFieldByName("fee_parus", currency_widget);
	grid->SetWidgetForFieldByName("outg_post", currency_widget);
	grid->SetWidgetForFieldByName("outg_daynight", currency_widget);

	creator.createAndAttachToGrid<CActDateCellWidget>("act_date", db_table);
	auto dt_widget = creator.createAndAttachToGrid<CDateCellWidget>(
													"act_reg_date", true);
	grid->SetWidgetForFieldByName("bank_reg_date", dt_widget);
	grid->SetWidgetForFieldByName("payment_date", dt_widget);
	grid->SetWidgetForFieldByName("cancel_date", dt_widget);
	grid->SetWidgetForFieldByName("bdate", dt_widget);
	grid->SetWidgetForFieldByName("rejection_date", dt_widget);

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

	const auto &params_manager = CParametersManager::getInstance();
	assert(params_manager.getDefaultCenter() < 10);
	std::string query = "SELECT id_user,user_full_name FROM users ";
	query += "WHERE id_center = ";
	query += '0' + params_manager.getDefaultCenter();
	query += " AND is_checker = 1 ";
	query += "ORDER BY user_name";
	stmt = conn->PrepareQuery(query.c_str());
	result_set = stmt->exec();
	rs_metadata = stmt->getResultSetMetadata();
	checkers_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
											"user_full_name", \
											conn, 1, result_set, rs_metadata, \
											"payments", db_table);
	//checkers_list->setMasterPrimaryKey("id_user");
	checkers_list->AddRelation("id_user", "id_checker");
}

void CSearchForm::adjustUIDependentCellWidgets() {

	assert(canceling_reasons_list);
	assert(act_no_list);

	canceling_reasons_list->AddItem(_T("скасовано"));
	canceling_reasons_list->AddItem(_T("припинено"));
	canceling_reasons_list->AddItem(_T("заміна"));
	canceling_reasons_list->AddItem(_T("відмова"));
	canceling_reasons_list->AddItem(_T("розбіжності"));
	canceling_reasons_list->AddItem(_T("хвороба"));
	canceling_reasons_list->AddItem(_T("розшук"));
	canceling_reasons_list->AddItem(_T("виключений з реєстру"));
	canceling_reasons_list->AddItem(_T("завершено"));

	act_no_list->AddItem(_T("Винагорода"));
	act_no_list->AddItem(_T("Витрати"));
}

void CSearchForm::loadInitialFilterToControls() {

	const auto &params_manager = CParametersManager::getInstance();

	flt_center->SetCurrRecord(params_manager.getDefaultCenter());

	auto &initial_date = params_manager.getInitialDateW();
	if (!initial_date.empty()) {
		flt_order_date_from->SetLabel(params_manager.getInitialDateW().c_str());
		flt_order_date_from->enableIfChanged();
	}
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

	flt_defender = new CFilteringDbComboBox(filtering_manager, \
											defenders_list->getMasterResultSet(), 2, 0);
	combo_binder = std::make_shared<CDbComboBoxBinderControl>(flt_defender);
	id_expr = filtering_manager.addExpr("a.id_defender = ?", combo_binder);
	flt_defender->setExprId(id_expr);

	flt_client = new CFilteringEdit(filtering_manager, this);
	auto str_binder = std::make_shared<CStringWidgetBinderControl>(flt_client);
	id_expr = filtering_manager.addExpr("a.client_name LIKE ?", str_binder);
	flt_client->setExprId(id_expr);

	flt_zone = new CZoneFilter(filtering_manager); 
	flt_act_no = new CActNoFilter(filtering_manager); 

	flt_act = new CFilteringEdit(filtering_manager, this);
	str_binder = std::make_shared<CStringWidgetBinderControl>(flt_act);
	id_expr = filtering_manager.addExpr("aa.id_act LIKE ?", str_binder);
	flt_act->setExprId(id_expr);

	flt_informer = new CFilteringDbComboBox(filtering_manager, \
											informers_list->getMasterResultSet(), 1, 0);
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

std::shared_ptr<IDbStatement> CSearchForm::createTableStmt() {

	const char* query = query_modifier.getQuery().c_str();
	int rec_id_index = fields_list.FIELDS_COUNT - 1;
	return std::make_shared<PaginalStatement>(conn, query, rec_id_index);
}

std::shared_ptr<CDbTable> CSearchForm::createDbTable() {

	std::string query = "SELECT ";

	query += fields_list.getFieldsList();
	query += " FROM orders a INNER JOIN payments aa ON a.id_center_legalaid = aa.id_center AND a.id = aa.id_order AND a.order_date = aa.order_date";
	query += " INNER JOIN people p ON a.id_defender = p.id_person";
	query += " INNER JOIN order_types t ON a.id_order_type = t.id_type";
	query += " INNER JOIN informers inf ON aa.id_informer = inf.id_inf";
	query += " INNER JOIN centers c ON a.id_center_legalaid = c.id_center";
	query += " INNER JOIN stages sta ON aa.id_stage = sta.id_st";
	query += " INNER JOIN users uu ON aa.id_checker = uu.id_user";
	query += " ####";
	query += " ORDER BY ";
	query += def_ordering_str;
	query_modifier.Init(query);

	const auto &params_manager = CParametersManager::getInstance();
	std::string initial_flt = params_manager.getInitialFilteringStr();
	query_modifier.changeWherePart(\
		ImmutableString<char>(initial_flt.c_str(), initial_flt.size()));

	auto stmt = createTableStmt();

	auto db_table = std::make_shared<CDbTable>(conn, stmt, false);
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
	db_table->markFieldAsPrimaryKey("act_no", "payments");
	
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
		sizer.addResizeableWidget(flt_defender, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(250, DEF_GUI_ROW_HEIGHT), 250);
		flt_defender->setTabStopManager(this);
		inserter.getOrdersInsertHelper().SetDefenderWidget(flt_defender);

		sizer.addWidget(new XLabel(), _T("Центр: "), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_center, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(180, DEF_GUI_ROW_HEIGHT), 200);
		flt_center->setTabStopManager(this);
		inserter.SetCenterBox(flt_center);

		sizer.addWidget(new XLabel(), _T("Інформатор: "), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_informer, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(270, DEF_GUI_ROW_HEIGHT), 250);
		flt_informer->setTabStopManager(this);
		inserter.getPaymentsInsertHelper().setInformerWidget(flt_informer);
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
		inserter.getOrdersInsertHelper().SetOrderTypeWidget(flt_order_type);

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
		sizer.addWidget(article, _T(""), edit_flags, XSize(340, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setArticleWidget(article);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Акт: "), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT));
		sizer.addWidget(flt_act, _T(""), FL_WINDOW_VISIBLE, \
						XSize(90, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setActWidget(flt_act);

		sizer.addWidget(new XLabel(), _T("Тип акту: "), FL_WINDOW_VISIBLE, \
						XSize(65, DEF_GUI_ROW_HEIGHT));
		flt_act_no->setTabStopManager(this);
		sizer.addResizeableWidget(flt_act_no, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(80, DEF_GUI_ROW_HEIGHT), 100);
		inserter.getPaymentsInsertHelper().setActNoWidget(flt_act_no);

		sizer.addWidget(new XLabel(), _T("Етап: "), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_stage, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(110, DEF_GUI_ROW_HEIGHT), 250);
		flt_stage->setTabStopManager(this);
		inserter.getPaymentsInsertHelper().setStageWidget(flt_stage);

		sizer.addWidget(new XLabel(), _T("№ розгляду:"), FL_WINDOW_VISIBLE, \
						XSize(90, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *cycle = new XTabStopEdit(this);
		sizer.addWidget(cycle, _T(""), edit_flags, XSize(30, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setCycleWidget(cycle);

		sizer.addWidget(new XLabel(), _T("Сума: "), FL_WINDOW_VISIBLE, \
						XSize(45, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *fee = new XCurrencyField(this);
		sizer.addWidget(fee, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(90, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setFeeWidget(fee);

		sizer.addWidget(new XLabel(), _T("Поштові: "), FL_WINDOW_VISIBLE, \
						XSize(62, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *outg_post = new XCurrencyField(this);
		sizer.addWidget(outg_post, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
						FL_EDIT_AUTOHSCROLL, XSize(50, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setOutgPostWidget(outg_post);

		sizer.addWidget(new XLabel(), _T("Добові: "), FL_WINDOW_VISIBLE, \
						XSize(55, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *outg_daynight = new XCurrencyField(this);
		sizer.addWidget(outg_daynight, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
						FL_EDIT_AUTOHSCROLL, XSize(50, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setOutgDayNightWidget(outg_daynight);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Дата розрах. з: "), FL_WINDOW_VISIBLE, \
						XSize(70, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_reg_date_from, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setActRegDateWidget(flt_act_reg_date_from);

		sizer.addWidget(new XLabel(), _T("Дата розрах. до: "), FL_WINDOW_VISIBLE, \
						XSize(80, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_reg_date_to, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
	
		sizer.addWidget(new XLabel(), _T("Дата акту з: "), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_date_from, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setActDateWidget(flt_act_date_from);

		sizer.addWidget(new XLabel(), _T("Дата акту до: "), FL_WINDOW_VISIBLE, \
						XSize(65, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_act_date_to, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Дата оплати з: "), FL_WINDOW_VISIBLE, \
						XSize(70, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_payment_date_from, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getPaymentsInsertHelper().setPaymentDateWidget(flt_payment_date_from);

		sizer.addWidget(new XLabel(), _T("Дата оплати до: "), FL_WINDOW_VISIBLE, \
						XSize(75, DEF_GUI_ROW_HEIGHT + 10));
		sizer.addWidget(flt_payment_date_to, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Клієнт:"), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
		sizer.addWidget(flt_client, _T(""), edit_flags, XSize(320, DEF_GUI_ROW_HEIGHT));
		inserter.getOrdersInsertHelper().SetClientWidget(flt_client);

		sizer.addWidget(new XLabel(), _T("Дата народж:"), FL_WINDOW_VISIBLE, \
						XSize(55, DEF_GUI_ROW_HEIGHT + 10));
		XDateField *bdate = new XDateField(this);
		sizer.addWidget(bdate, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));
		inserter.getOrdersInsertHelper().SetClientBirthDateWidget(bdate);

		sizer.addWidget(new XLabel(), _T("Зона відп.:"), FL_WINDOW_VISIBLE, \
						XSize(85, DEF_GUI_ROW_HEIGHT + 10));
		flt_zone->setTabStopManager(this);
		sizer.addResizeableWidget(flt_zone, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(50, DEF_GUI_ROW_HEIGHT), 100);
		
		sizer.addWidget(new XLabel(), _T("Оплачено:"), FL_WINDOW_VISIBLE, \
						XSize(85, DEF_GUI_ROW_HEIGHT + 10));
		flt_paid->setTabStopManager(this);
		sizer.addResizeableWidget(flt_paid, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(50, DEF_GUI_ROW_HEIGHT), 100);

		auto checkers_list_ctl = new CDbComboBox(checkers_list->getMasterResultSet(), 1, 0);
		sizer.addResizeableWidget(checkers_list_ctl, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
									XSize(110, DEF_GUI_ROW_HEIGHT), 250);
		checkers_list_ctl->setTabStopManager(this);
		inserter.getPaymentsInsertHelper().setCheckerWidget(checkers_list_ctl);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		btn_apply_filter = new XButton();
		sizer.addWidget(btn_apply_filter, _T("Фільтр"), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));

		btn_reset = new XButton();
		sizer.addWidget(btn_reset, _T("Скд"), FL_WINDOW_VISIBLE, \
						XSize(35, DEF_GUI_ROW_HEIGHT));
		
		auto db_navigator = new CDbNavigator(db_table);
		sizer.addWidget(db_navigator, _T(""), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));

		btn_add = new XButton();
		sizer.addWidget(btn_add, _T("+"), FL_WINDOW_VISIBLE, \
						XSize(30, DEF_GUI_ROW_HEIGHT));

		auto &perm_mgr = CParametersManager::getInstance().getPermissions();
		int rm_button_flags = FL_WINDOW_VISIBLE * \
							(perm_mgr.isOrdersDeleter() || perm_mgr.isAdmin() || \
								perm_mgr.isLocalAdmin());
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
						XSize(50, DEF_GUI_ROW_HEIGHT));
		
		btn_upload = new XButton();
		sizer.addWidget(btn_upload, _T("Вивант"), FL_WINDOW_VISIBLE, \
						XSize(55, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	XRect grid_coords = main_sizer.addLastWidget(grid);

	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_stage"));
	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_center_legalaid"));
	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_defender"));
	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_order_type"));
	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_informer"));
	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_checker"));
	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("rec_id"));
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
	const auto &meta_info = db_table->getMetaInfo();
	ordering_box->addOrderingField("zone", meta_info, grid);
	ordering_box->addOrderingField("center_short_name", meta_info, grid);
	ordering_box->addOrderingField("name_short", meta_info, grid);
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
	ordering_box->addOrderingField("rejection_date", meta_info, grid);
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

		auto stmt = createTableStmt();
		
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

		auto stmt = createTableStmt();
		auto stmt_aggregate = conn->PrepareQuery(query_aggregate.getQuery().c_str());
		def_binding_target->replaceFirst(stmt);
		def_binding_target->replaceSecond(stmt_aggregate);

		filtering_manager.apply(def_binding_target);
		db_table->reload(stmt);
		reloadStatisticsControls(stmt_aggregate);
	}
	else {
		filtering_manager.apply(def_binding_target);
		db_table->reload();
		reloadStatisticsControls();
	}
}

void CSearchForm::OnAddRecordButtonClick(XCommandEvent *eve) {

	if (inserter.insert()) {
		db_table->reload();
		reloadStatisticsControls();
	}
}

void CSearchForm::OnRemoveButtonClick(XCommandEvent *eve) {

	db_table->gotoCurrentRecord();
	auto &perm_mgr = CParametersManager::getInstance().getPermissions();
	if (!perm_mgr.isAdmin() && constraints->wrong_zone) {
		ErrorBox(_T("Видаляти доручення/стадії інших центрів заборонено"));
		return;
	}

	int option = _plMessageBoxYesNo(_T("Видалити поточний запис?"));
	if (option == IDYES) {
		std::string query;
		bool is_null = false;
		const auto &meta_info = db_table->getMetaInfo();

		query.reserve(200);
		query = "SELECT COUNT(*) FROM payments ";
		query += "WHERE id_center = ? AND id_order = ? AND order_date = ?";
		
		auto stmt = conn->PrepareQuery(query.c_str());
		auto rs_main = db_table->getResultSet();
		
		const size_t center = meta_info.getFieldIndexByName("id_center_legalaid");
		const size_t id = meta_info.getFieldIndexByName("id", "orders");
		const size_t order_date = meta_info.getFieldIndexByName("order_date", "orders");

		stmt->bindValue(0, rs_main->getInt(center, is_null));
		stmt->bindValue(1, rs_main->getInt(id, is_null));
		stmt->bindValue(2, rs_main->getDate(order_date, is_null));
		auto rs = stmt->exec();

		assert(!rs->empty());
		rs->gotoRecord(0);
		const size_t stages_count = (size_t)rs->getInt(0, is_null);

		assert(!is_null);
		assert(stages_count);
		const char *table = stages_count > 1 ? "payments" : "orders";

		meta_info.getDeleteQuery(table, query);
		auto del_stmt = conn->PrepareQuery(query.c_str());
		meta_info.bindPrimaryKeyValues(table, rs_main, del_stmt);
		del_stmt->execScalar();

		db_table->reload();
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

	inserter.resetControls();
	OnFilterButtonClick(eve);
}

void CSearchForm::OnUploadButtonClick(XCommandEvent *eve) {

	try {
		uploader.upload(grid);
	}
	catch (XException &e) {
		ErrorBox(e.what());
		return;
	}
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

	if (flt_act_no && !flt_act_no->IsCreated()) delete flt_act_no;
	if (flt_zone && !flt_zone->IsCreated()) delete flt_zone;
	if (flt_paid && !flt_paid->IsCreated()) delete flt_paid;
	if (flt_order_type && !flt_order_type->IsCreated()) delete flt_order_type;
	if (flt_stage && !flt_stage->IsCreated()) delete flt_stage;
	if (flt_center && !flt_center->IsCreated()) delete flt_center;
	if (flt_client && !flt_client->IsCreated()) delete flt_client;
	if (flt_informer && !flt_informer->IsCreated()) delete flt_informer;
	if (flt_defender && !flt_defender->IsCreated()) delete flt_defender;
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
