#include <basic/XConv.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_ext/DependentTableUpdater.h>
#include <xwindows/XLabel.h>
#include <xwindows/XButton.h>
#include <xwindows_ex/HorizontalSizer.h>
#include <xwindows_ex/VerticalSizer.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <db_controls/FilteringEdit.h>
#include <db_controls/FilteringDbComboBox.h>
#include <db_controls/BinderControls.h>
#include <db_controls/DbNavigator.h>
#include "SearchForm.h"

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

CSearchForm::CSearchForm(XWindow *parent, const int flags, \
					const Tchar *label, \
					const int X, const int Y, \
					const int width, const int height) : \
				flt_id(nullptr), grid(nullptr), advocats_list(nullptr), \
				centers_list(nullptr), \
				grid_x(0), grid_y(0), grid_margin_x(0), grid_margin_y(0) {

	props.open("config.ini");

	conn = CMySQLConnectionFactory::createConnection(props);
	db_table = createDbTable(conn);

	grid = new CDbGrid(false, db_table);
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(grid);
	initFilteringControls();

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	displayWidgets();
	adjustUIDependentCellWidgets(grid);

	Connect(EVT_COMMAND, btn_apply_filter->GetId(), this, &CSearchForm::OnFilterButtonClick);
	Connect(EVT_COMMAND, btn_add->GetId(), this, &CSearchForm::OnAddRecordButtonClick);
	Connect(EVT_COMMAND, btn_remove->GetId(), this, &CSearchForm::OnRemoveButtonClick);
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
	grid->SetFieldWidth(5, 4);
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
	grid->SetFieldWidth(14, 15);
	grid->SetFieldLabel(14, _T("Акт"));
	grid->SetFieldLabel(15, _T("Дт розрах"));
	grid->SetFieldLabel(16, _T("Дт акту"));
	grid->SetFieldLabel(17, _T("Дт р. казн"));
	grid->SetFieldLabel(18, _T("Дата опл."));
	grid->SetFieldWidth(19, 1);
	grid->SetFieldLabel(19, _T("#"));
	grid->SetFieldWidth(20, 30);
	grid->SetFieldLabel(20, _T("Стаття"));
	grid->SetFieldWidth(21, 32);
	grid->SetFieldLabel(21, _T("Інформатор"));
}

void CSearchForm::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {

	assert(grid);
	assert(!advocats_list);
	assert(!centers_list);
	CGridCellWidgetCreator creator(grid);
	
	advocats_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"adv_name_short", \
										conn, "adv_name_short", "advocats", \
										"orders", db_table);
	advocats_list->AddRelation("id_advocat", "id_adv");

	centers_list = creator.createAndAttachToGrid<CDbComboBoxCellWidget>(\
										"center_short_name", \
										conn, "center_short_name", "centers", \
										"orders", db_table);
	centers_list->AddRelation("id_center", "id_center_legalaid");
}

void CSearchForm::adjustUIDependentCellWidgets(CDbGrid *grid) {}

void CSearchForm::initFilteringControls() {

	flt_id = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> id_binder = std::make_shared<CIntWidgetBinderControl>(flt_id);
	ImmutableString<char> expr("a.id = ?", sizeof("a.id = ?") - 1);
	int id_expr = filtering_manager.addExpr(expr, id_binder);
	flt_id->setExprId(id_expr);

	flt_advocat = new CFilteringDbComboBox(filtering_manager, \
											advocats_list->getMasterResultSet(), 1, 0);
	auto adv_binder = std::make_shared<CDbComboBoxBinderControl>(flt_advocat);
	id_expr = filtering_manager.addExpr(\
			ImmutableString<char>("a.id_adv = ?", sizeof("a.id_adv = ?") - 1), adv_binder);
	flt_advocat->setExprId(id_expr);

	flt_center = new CFilteringDbComboBox(filtering_manager, \
											centers_list->getMasterResultSet(), 2, 0);
}

std::shared_ptr<CDbTable> CSearchForm::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT a.zone, c.center_short_name, b.adv_name_short, a.id, a.order_date,";
	query += " t.type_name, a.client_name, a.bdate, sta.stage_name, a.reason, a.cancel_order, a.cancel_date, aa.fee, aa.outgoings,";
	query += " aa.id_act, aa.act_reg_date, aa.act_date, aa.bank_reg_date, aa.payment_date,";
	query += " aa.cycle, aa.article, inf.informer_name, aa.id_stage,";
	query += " a.id_center_legalaid, a.id_adv, a.id_order_type, aa.id_informer ";
	query += "FROM orders a INNER JOIN payments aa ON a.id_center_legalaid = aa.id_center AND a.id = aa.id_order AND a.order_date = aa.order_date";
	query += " INNER JOIN advocats b ON a.id_adv = b.id_advocat";
	query += " INNER JOIN order_types t ON a.id_order_type = t.id_type";
	query += " INNER JOIN informers inf ON aa.id_informer = inf.id_inf";
	query += " INNER JOIN centers c ON a.id_center_legalaid = c.id_center";
	query += " INNER JOIN stages sta ON aa.id_stage = sta.id_st";
	query += " #### ";
	query += " ORDER BY a.id_center_legalaid,a.order_date,a.id,aa.cycle,aa.id_stage";
	query_modifier.Init(query);

	auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("payments");
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

		sizer.addWidget(new XLabel(), _T("Центр: "), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT));
		sizer.addResizeableWidget(flt_center, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(180, DEF_GUI_ROW_HEIGHT), 200);

	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("№ дор.: "), FL_WINDOW_VISIBLE, \
						XSize(60, DEF_GUI_ROW_HEIGHT));
		sizer.addWidget(flt_id, _T(""), FL_WINDOW_VISIBLE, \
						XSize(45, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		btn_apply_filter = new XButton();
		sizer.addWidget(btn_apply_filter, _T("Фільтр"), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));

		auto db_navigator = new CDbNavigator(db_table);
		sizer.addWidget(db_navigator, _T(""), FL_WINDOW_VISIBLE, \
						XSize(100, DEF_GUI_ROW_HEIGHT));

		btn_add = new XButton();
		sizer.addWidget(btn_add, _T("+"), FL_WINDOW_VISIBLE, \
						XSize(30, DEF_GUI_ROW_HEIGHT));

		btn_remove = new XButton();
		sizer.addWidget(btn_remove, _T("-"), FL_WINDOW_VISIBLE, \
						XSize(30, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	XRect grid_coords = main_sizer.addLastWidget(grid);

	grid->HideField(22);
	grid->HideField(23);
	grid->HideField(24);
	grid->HideField(25);
	grid->HideField(26);
	grid->SetFocus();

	grid_x = grid_coords.left;
	grid_y = grid_coords.top;

	XRect margins = main_sizer.getMargins();
	grid_margin_x = margins.left;
	grid_margin_y = margins.top;
}

void CSearchForm::OnFilterButtonClick(XCommandEvent *eve) {

	if (filtering_manager.isFilteringStringChanged()) {

		query_modifier.changeWherePart(filtering_manager.buildFilteringString());

		auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());
		filtering_manager.apply(stmt);
		db_table->reload(stmt);
	}
	else {

		filtering_manager.apply(db_table->getBindingTarget());
		db_table->reload();
	}
}

void CSearchForm::OnAddRecordButtonClick(XCommandEvent *eve) {


}

void CSearchForm::OnRemoveButtonClick(XCommandEvent *eve) {

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
	if (flt_id && !flt_id->IsCreated()) delete flt_id;
}
