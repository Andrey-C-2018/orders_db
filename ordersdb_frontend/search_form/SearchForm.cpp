#include <basic/XConv.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_ext/DependentTableUpdater.h>
#include <xwindows/XButton.h>
#include <db_controls/DbGrid.h>
#include <db_controls/FilteringEdit.h>
#include <db_controls/BinderControls.h>
#include "SearchForm.h"

CSearchForm::CSearchForm(XWindow *parent, const int flags, \
					const Tchar *label, \
					const int X, const int Y, \
					const int width, const int height) {

	props.open("config.ini");

	initFilteringControls();

	conn = CMySQLConnectionFactory::createConnection(props);
	db_table = createDbTable(conn);

	grid = new CDbGrid(false, db_table);
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(grid);

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
		label, X, Y, width, height);
	displayInputWidgets();
	adjustUIDependentCellWidgets(grid);

	Connect(EVT_COMMAND, btn_apply_filter->GetId(), this, &CSearchForm::OnFilterButtonClick);
	Connect(EVT_COMMAND, btn_add->GetId(), this, &CSearchForm::OnAddRecordButtonClick);
	Connect(EVT_COMMAND, btn_remove->GetId(), this, &CSearchForm::OnRemoveButtonClick);

}

void CSearchForm::setFieldsSizes() {}

void CSearchForm::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {}

void CSearchForm::adjustUIDependentCellWidgets(CDbGrid *grid) {}

void CSearchForm::initFilteringControls() {

	flt_id = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> id_binder = std::make_shared<CIntWidgetBinderControl>(flt_id);

	ImmutableString<char> expr("b.id_advocat = ?", sizeof("b.id_advocat = ?") - 1);
	int id_expr = filtering_manager.addExpr(expr, id_binder);
	flt_id->setExprId(id_expr);
}

std::shared_ptr<CDbTable> CSearchForm::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT a.zone, c.center_short_name, b.adv_name_short, a.id, a.order_date,";
	query += " t.type_name, a.client_name, a.bdate, sta.stage_name, a.reason, a.cancel_order, a.cancel_date, aa.fee, aa.outgoings,";
	query += " aa.id_act, aa.act_reg_date, aa.act_date, aa.bank_reg_date, aa.payment_date,";
	query += " aa.cycle, aa.article, inf.informer_name, aa.id_stage, aa.id_stage,";
	query += " a.id_center_legalaid, a.id_adv, a.id_order_type, aa.id_informer";
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

	//db_table->markFieldAsPrimaryKey("id_center", "payments", \
	//								"id_center_legalaid", "orders");
	//db_table->addPrimaryKeyAsRef("id_order", "payments", \
	//								"id", "orders");
	//db_table->addPrimaryKeyAsRef("order_date", "payments", \
	//								"order_date", "orders");
	db_table->markFieldAsPrimaryKey("id_stage", "payments");
	db_table->markFieldAsPrimaryKey("cycle", "payments");
	
	return db_table;
}

void CSearchForm::displayInputWidgets() {


}

void CSearchForm::OnFilterButtonClick(XCommandEvent *eve) {


}

void CSearchForm::OnAddRecordButtonClick(XCommandEvent *eve) {

}

void CSearchForm::OnRemoveButtonClick(XCommandEvent *eve) {

}

void CSearchForm::OnSize(XSizeEvent *eve) { }

CSearchForm::~CSearchForm() { }
