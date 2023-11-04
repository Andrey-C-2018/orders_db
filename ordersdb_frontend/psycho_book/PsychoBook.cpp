#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <xwindows/XLabel.h>
#include <xwindows_ex/HorizontalSizer.h>
#include <xwindows_ex/VerticalSizer.h>
#include <xwindows_ex/XTabStopWidget.h>
#include <xwindows_ex/XTabStopEdit.h>
#include <xwindows_ex/XDateField.h>
#include <editable_grid/DateCellWidget.h>
#include <db_controls/DbGrid.h>
#include <db_controls/BinderControls.h>
#include <db_controls/FilteringEdit.h>
#include <db_controls/DbNavigator.h>
#include <forms_common/VernamOneTimePad.h>
#include "PsychoBook.h"

PsychoBook::PsychoBook(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int X, const int Y, \
							const int width, const int height) : \
					flt_id(nullptr), flt_name(nullptr), \
					btn_apply_filter(nullptr), btn_add(nullptr), btn_remove(nullptr), \
					grid_x(0), grid_y(0), grid_margin_x(0), grid_margin_y(0) {

	props.open();

	initFilteringControls();

	conn = CMySQLConnectionFactory::createConnection(props, vernam_one_time_pad);
	db_table = createDbTable(conn);
	
	grid = new CDbGrid(false, db_table);
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(grid);

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	DisplayWidgets();

	Connect(EVT_COMMAND, btn_apply_filter->GetId(), this, &PsychoBook::OnFilterButtonClick);
	Connect(EVT_COMMAND, btn_add->GetId(), this, &PsychoBook::OnAddRecordButtonClick);
	Connect(EVT_COMMAND, btn_remove->GetId(), this, &PsychoBook::OnRemoveButtonClick);
}
	
std::shared_ptr<CDbTable> PsychoBook::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_psycho, p.name, p.name_short, ";
	query += " b.address, b.tel, b.email, p.bdate ";
	query += "FROM psychologists b INNER JOIN people p ON b.id_psycho = p.id_person";
	query += " #### ";
	query += "ORDER BY name";
	query_modifier.Init(query);

	auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());

	auto db_table = std::make_shared<CDbTable>(conn, stmt);
	db_table->setPrimaryTableForQuery("people");
	db_table->markFieldAsPrimaryKey("id_psycho", "psychologists");

	db_table->addPrimaryKeyAsRef("id_person", "people", "id_psycho", "psychologists");
	return db_table;
}

void PsychoBook::setFieldsSizes() {

	const auto& meta_info = db_table->getMetaInfo();
	size_t field_index = meta_info.getFieldIndexByName("id_psycho");

	grid->SetFieldWidth(field_index, 4);
	grid->SetFieldLabel(field_index, _T("ID"));

	field_index = meta_info.getFieldIndexByName("name");
	grid->SetFieldWidth(field_index, 37);
	grid->SetFieldLabel(field_index, _T("ПІБ психолога"));

	field_index = meta_info.getFieldIndexByName("name_short");
	grid->SetFieldWidth(field_index, 25);
	grid->SetFieldLabel(field_index, _T("ПІБ коротке"));

	field_index = meta_info.getFieldIndexByName("address");
	grid->SetFieldWidth(meta_info.getFieldIndexByName("address"), 75);
	grid->SetFieldLabel(field_index, _T("Адреса"));

	grid->SetFieldWidth(meta_info.getFieldIndexByName("tel"), 25);
	grid->SetFieldWidth(meta_info.getFieldIndexByName("email"), 20);

	field_index = meta_info.getFieldIndexByName("bdate");
	grid->SetFieldWidth(field_index, 12);
	grid->SetFieldLabel(field_index, _T("Дата народж"));
}

void PsychoBook::createCellWidgetsAndAttachToGrid(CDbGrid* grid) {

	assert(grid);
	CEditableCellWidget* readonly_widget = nullptr;
	CDateCellWidget *date_widget = nullptr;

	bool date = false, readonly = false;
	try {
		readonly_widget = new CEditableCellWidget(true);
		grid->SetWidgetForFieldByName("id_psycho", readonly_widget);
		readonly = true;

		date_widget = new CDateCellWidget();
		grid->SetWidgetForFieldByName("bdate", date_widget);
		date = true;
	}
	catch (...) {
		if (!date && date_widget && !date_widget->IsCreated())
			delete date_widget;
		if (!readonly && readonly_widget && !readonly_widget->IsCreated())
			delete readonly_widget;

		throw;
	}
}

void PsychoBook::DisplayWidgets() {

	XRect rc;
	const int edit_flags = FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
							FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL;

	GetClientRect(rc);
	CVerticalSizer main_sizer(this, 0, 0, rc.right, rc.bottom, \
								10, 10, 10, 10, \
								DEF_GUI_VERT_GAP, DEF_GUI_ROW_HEIGHT);

	CHorizontalSizer sizer(CSizerPreferences(0, 0, 0, 0, DEF_GUI_HORZ_GAP));
	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("ID: "), FL_WINDOW_VISIBLE, \
						XSize(20, DEF_GUI_ROW_HEIGHT));
		sizer.addWidget(flt_id, _T(""), FL_WINDOW_VISIBLE, \
						XSize(45, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("ПІБ: "), FL_WINDOW_VISIBLE, \
						XSize(30, DEF_GUI_ROW_HEIGHT));
		inserter.addFieldValueHolder("name", flt_name);
		sizer.addWidget(flt_name, _T(""), edit_flags, XSize(280, DEF_GUI_ROW_HEIGHT));
		
		sizer.addWidget(new XLabel(), _T("Адреса:"), FL_WINDOW_VISIBLE, \
						XSize(53, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *address = new XTabStopEdit(this);
		inserter.addFieldValueHolder("address", address);
		sizer.addWidget(address, _T(""), edit_flags, XSize(380, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Дата народж:"), FL_WINDOW_VISIBLE, \
						XSize(55, DEF_GUI_ROW_HEIGHT + 10));
		XDateField* bdate = new XDateField(this);
		inserter.addFieldValueHolder("bdate", bdate);
		sizer.addWidget(bdate, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Тел.:"), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *tel = new XTabStopEdit(this);
		inserter.addFieldValueHolder("tel", tel);
		sizer.addWidget(tel, _T(""), edit_flags, XSize(200, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("E-mail:"), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *email = new XTabStopEdit(this);
		inserter.addFieldValueHolder("email", email);
		sizer.addWidget(email, _T(""), edit_flags, XSize(200, DEF_GUI_ROW_HEIGHT));
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

	grid->SetFocus();

	grid_x = grid_coords.left;
	grid_y = grid_coords.top;

	XRect margins = main_sizer.getMargins();
	grid_margin_x = margins.left;
	grid_margin_y = margins.top;

	inserter.prepare(conn);
}

void PsychoBook::initFilteringControls() {

	flt_id = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> id_binder = std::make_shared<CIntWidgetBinderControl>(flt_id);
	
	ImmutableString<char> expr("b.id_psycho = ?", sizeof("b.id_psycho = ?") - 1);
	int id_expr = filtering_manager.addExpr(expr, id_binder);
	flt_id->setExprId(id_expr);

	flt_name = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> name_binder = std::make_shared<CStringWidgetBinderControl>(flt_name);

	ImmutableString<char> expr2("p.name LIKE ?", sizeof("p.name LIKE ?") - 1);
	int name_expr = filtering_manager.addExpr(expr2, name_binder);
	flt_name->setExprId(name_expr);
}

void PsychoBook::OnFilterButtonClick(XCommandEvent *eve) {

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

void PsychoBook::OnAddRecordButtonClick(XCommandEvent *eve) {

	if (inserter.insert())
		db_table->reload();
}

void PsychoBook::OnRemoveButtonClick(XCommandEvent *eve) {

	int option = _plMessageBoxYesNo(_T("Видалити поточний запис?"));
	if (option == IDYES) {
		try {
			db_table->removeCurrentRecord();
		}
		catch (CDbException& e) {
			if (e.E_DB_PARENT_NO_ROW_UPD_DEL)
				ErrorBox(_T("до цього психолога прив'язані доручення, тому видалення неможливе"));
			else
				ErrorBox(e.what());
		}
	}
}

void PsychoBook::OnSize(XSizeEvent *eve) {

	int width = eve->GetWidth();
	int height = eve->GetHeight();

	if (!(grid && width && height)) return;
		
	grid->MoveWindow(grid_x, grid_y, \
					width - 2 * grid_margin_x, \
					height - grid_y - grid_margin_y);
}

PsychoBook::~PsychoBook() {

	if (grid && !grid->IsCreated()) delete grid;
	if (flt_id && !flt_id->IsCreated()) delete flt_id;
	if (flt_name && !flt_name->IsCreated()) delete flt_name;
}
