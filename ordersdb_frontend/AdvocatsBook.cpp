#include <db_controls/DbGrid.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/MySQL/MySQLConnection.h>
#include <xwindows/HorizontalSizer.h>
#include <xwindows/VerticalSizer.h>
#include "AdvocatsBook.h"
#include "BinderControls.h"

CAdvocatsBook::CAdvocatsBook(const Tchar *class_name, \
								const Tchar *label, const int X, const int Y, \
								const int width, const int height) : \
	flt_id(nullptr), flt_id_changed(false), btn_apply_filter(nullptr), btn_ordering(nullptr), \
	btn_add(nullptr), btn_remove(nullptr), btn_upload(nullptr), grid(nullptr), \
	adv_org_types_list(nullptr), \
	grid_x(0), grid_y(0), grid_margin_x(0), grid_margin_y(0) {

	props.open("config.ini");

	initBinderControls();

	conn = CMySQLConnectionFactory::createConnection(props);
	db_table = createDbTable(conn);
	
	grid = new CDbGrid(db_table);
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(grid);

	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	DisplayWidgets();
	adjustUIDependentCellWidgets(grid);

	Connect(EVT_SIZE, this, &CAdvocatsBook::OnSize);
	Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
				flt_id->GetId(), this, &CAdvocatsBook::OnFilteringWidgetChanged);
	Connect(EVT_COMMAND, NCODE_EDIT_LOOSE_FOCUS, \
				flt_id->GetId(), this, &CAdvocatsBook::OnFilteringWidgetLooseFocus);
	Connect(EVT_COMMAND, btn_apply_filter->GetId(), this, &CAdvocatsBook::OnFilterButtonClick);
}
	
std::shared_ptr<CDbTable> CAdvocatsBook::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_advocat, b.adv_name, b.license_no, b.license_date,";
	query += " e.exm_name, b.post_index, b.address, b.adv_bdate,";
	query += " d.distr_center, b.org_name, b.org_type ";
	query += "FROM advocats b INNER JOIN examiners e ON b.id_exm = e.id_examiner";
	query += " INNER JOIN districts d ON b.id_main_district = d.id_distr";
	query += " #### ";
	query += "ORDER BY adv_name";
	query_modifier.Init(query);

	auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("advocats");
	db_table->markFieldAsPrimaryKey("id_advocat");

	return db_table;
}

void CAdvocatsBook::setFieldsSizes() {

	grid->SetFieldWidth(0, 4);
	grid->SetFieldWidth(1, 37);
	grid->SetFieldWidth(2, 10);
	grid->SetFieldWidth(4, 70);
	grid->SetFieldWidth(5, 6);
	grid->SetFieldWidth(6, 75);
	grid->SetFieldWidth(8, 25);
	grid->SetFieldWidth(9, 55);
}

void CAdvocatsBook::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {

	assert(grid);
	auto examiners_list = new CDbComboBoxCellWidget(conn, "exm_name", \
												"examiners", "advocats", db_table);
	examiners_list->AddRelation("id_examiner", "id_exm");
	grid->SetWidgetForFieldByName("exm_name", examiners_list);

	adv_org_types_list = new CComboBoxCellWidget();
	grid->SetWidgetForFieldByName("org_type", adv_org_types_list);
}

void CAdvocatsBook::adjustUIDependentCellWidgets(CDbGrid *grid) {

	assert(adv_org_types_list);
	adv_org_types_list->AddItem(_T("²ÍÄ"));
	adv_org_types_list->AddItem(_T("ÀÎ"));
	adv_org_types_list->AddItem(_T("ÀÁ"));
}

void CAdvocatsBook::DisplayWidgets() {

	XRect rc;

	GetClientRect(rc);
	CVerticalSizer main_sizer(this, 0, 0, rc.right, rc.bottom, \
								10, 10, 10, 10, \
								DEF_GUI_VERT_GAP, DEF_GUI_ROW_HEIGHT);
	main_sizer.addWidget(flt_id, _T(""), FL_WINDOW_VISIBLE);

	CHorizontalSizer sizer(CSizerPreferences(0, 0, 0, 0, DEF_GUI_HORZ_GAP));
	main_sizer.pushNestedSizer(sizer);
	btn_apply_filter = new XButton();
	sizer.addWidget(btn_apply_filter, _T("Ô³ëüòð"), FL_WINDOW_VISIBLE, \
					XSize(100, DEF_GUI_ROW_HEIGHT));
	btn_ordering = new XButton();
	sizer.addWidget(btn_ordering, _T("Ñîðò."), FL_WINDOW_VISIBLE, \
					XSize(100, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	XRect grid_coords = main_sizer.addLastWidget(grid);
	grid->SetFocus();

	grid_x = grid_coords.left;
	grid_y = grid_coords.top;

	XRect margins = main_sizer.getMargins();
	grid_margin_x = margins.left;
	grid_margin_y = margins.top;
}

void CAdvocatsBook::initBinderControls() {

	flt_id = new XEdit();
	std::shared_ptr<IBinder> id_binder = std::make_shared<CIntWidgetBinderControl>(flt_id);
	ImmutableString<char> expr("b.id_advocat = ?", sizeof("b.id_advocat = ?") - 1);

	int id_expr = filtering_manager.addExpr(expr, id_binder);
	flt_id->SetTag(id_expr);
}

void CAdvocatsBook::OnFilteringWidgetChanged(XCommandEvent *eve) {

	flt_id_changed = true;
}

void CAdvocatsBook::OnFilteringWidgetLooseFocus(XCommandEvent *eve) {

	if (flt_id_changed) {
		int id_expr = eve->GetSender()->GetTag();

		const Tchar *label = eve->GetSender()->GetLabel();
		if(label && label[0] != _T('\0'))
			filtering_manager.enableExpr(id_expr);
		else
			filtering_manager.disableExpr(id_expr);

		flt_id_changed = false;
	}
}

void CAdvocatsBook::OnFilterButtonClick(XCommandEvent *eve) {

	if (filtering_manager.isWherePartChanged()) {

		query_modifier.changeWherePart(filtering_manager.getSQLWherePart());
		
		auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());
		filtering_manager.apply(stmt);
		db_table->reload(stmt);
	}
	else {

		filtering_manager.apply(db_table->getBindingTarget());
		db_table->reload();
	}
}

void CAdvocatsBook::OnOrderingButtonClick(XCommandEvent *eve) {

}

void CAdvocatsBook::OnRemoveButtonClick(XCommandEvent *eve) {

}

void CAdvocatsBook::OnUploadButtonClick(XCommandEvent *eve) {

}

void CAdvocatsBook::OnSize(XSizeEvent *eve) {

	int width = eve->GetWidth();
	int height = eve->GetHeight();

	if (grid) grid->MoveWindow(grid_x, grid_y, \
		width - 2 * grid_margin_x, \
		height - grid_y - grid_margin_y);
}

CAdvocatsBook::~CAdvocatsBook() { }
