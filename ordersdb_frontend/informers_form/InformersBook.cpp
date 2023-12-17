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
#include <db_controls/DbComboBox.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <forms_common/VernamOneTimePad.h>
#include <forms_common/ParametersManager.h>
#include <forms_common/CommonRoutines.h>
#include "InformersBook.h"

typedef XTabStopWidget<XComboBox> XTabStopComboBox;

InformersBook::InformersBook(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int X, const int Y, \
							const int width, const int height) : \
					flt_id(nullptr), flt_informer(nullptr), \
					districts_list(nullptr), org_types_list(nullptr), \
					btn_apply_filter(nullptr), btn_add(nullptr), btn_remove(nullptr), \
					grid_x(0), grid_y(0), grid_margin_x(0), grid_margin_y(0) {

	props.open();

	initFilteringControls();

	conn = CMySQLConnectionFactory::createConnection(props, vernam_one_time_pad);
	CParametersManager::init(&props, conn);
	checkIsAdmin();

	db_table = createDbTable(conn);
	
	grid = new CDbGrid(false, db_table);
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(grid);

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	DisplayWidgets();
	adjustUIDependentCellWidgets(grid);

	Connect(EVT_COMMAND, btn_apply_filter->GetId(), this, &InformersBook::OnFilterButtonClick);
	Connect(EVT_COMMAND, btn_add->GetId(), this, &InformersBook::OnAddRecordButtonClick);
	Connect(EVT_COMMAND, btn_remove->GetId(), this, &InformersBook::OnRemoveButtonClick);
}
	
std::shared_ptr<CDbTable> InformersBook::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT inf.id_inf, inf.informer_name, inf.informer_full_name,";
	query += " d.distr_center, inf.inf_class, inf.id_location ";
	query += "FROM informers inf INNER JOIN districts d ON inf.id_location = d.id_distr";
	query += " #### ";
	query += "ORDER BY informer_name";
	query_modifier.Init(query);

	auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());

	auto db_table = std::make_shared<CDbTable>(conn, stmt);
	db_table->setPrimaryTableForQuery("informers");
	db_table->markFieldAsPrimaryKey("id_inf", "informers");

	return db_table;
}

void InformersBook::setFieldsSizes() {

	const auto& meta_info = db_table->getMetaInfo();
	size_t field_index = meta_info.getFieldIndexByName("id_inf");

	grid->SetFieldWidth(field_index, 5);
	grid->SetFieldLabel(field_index, _T("ID"));

	field_index = meta_info.getFieldIndexByName("informer_name");
	grid->SetFieldWidth(field_index, 40);
	grid->SetFieldLabel(field_index, _T("Інформатор скорочено"));

	field_index = meta_info.getFieldIndexByName("informer_full_name");
	grid->SetFieldWidth(field_index, 60);
	grid->SetFieldLabel(field_index, _T("Інформатор повн."));

	field_index = meta_info.getFieldIndexByName("distr_center");
	grid->SetFieldWidth(field_index, 22);
	grid->SetFieldLabel(field_index, _T("Місцезнаходження"));

	field_index = meta_info.getFieldIndexByName("inf_class");
	grid->SetFieldWidth(field_index, 25);
	grid->SetFieldLabel(field_index, _T("Тип"));
}

void InformersBook::createCellWidgetsAndAttachToGrid(CDbGrid* grid) {

	assert(grid);
	assert(!districts_list);
	assert(!org_types_list);
	CEditableCellWidget* readonly_widget = nullptr;

	bool readonly = false, distr = false, org_types = false;
	try {
		readonly_widget = new CEditableCellWidget(true);
		grid->SetWidgetForFieldByName("id_inf", readonly_widget);
		readonly = true;

		districts_list = new CDbComboBoxCellWidget(conn, "distr_center", \
														"districts", "informers", db_table);
		districts_list->AddRelation("id_distr", "id_location");
		grid->SetWidgetForFieldByName("distr_center", districts_list);
		distr = true;

		org_types_list = new CComboBoxCellWidget();
		grid->SetWidgetForFieldByName("inf_class", org_types_list);
		org_types = true;
	}
	catch (...) {
		if (!org_types && org_types_list && !org_types_list->IsCreated())
			delete org_types_list;
		if (!distr && districts_list && !districts_list->IsCreated())
			delete districts_list;
		if (!readonly && readonly_widget && !readonly_widget->IsCreated())
			delete readonly_widget;

		throw;
	}
}

void InformersBook::adjustUIDependentCellWidgets(CDbGrid* grid) {

	assert(org_types_list);
	org_types_list->AddItem(_T("Правоохоронні органи"));
	org_types_list->AddItem(_T("Пенітенціарні установи"));
	org_types_list->AddItem(_T("Суди"));
	org_types_list->AddItem(_T("Інші установи"));
}

void InformersBook::DisplayWidgets() {

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

		sizer.addWidget(new XLabel(), _T("Інформатор скор: "), FL_WINDOW_VISIBLE, \
						XSize(125, DEF_GUI_ROW_HEIGHT));
        inserter.addFieldValueHolder("informer_name", flt_informer);
		sizer.addWidget(flt_informer, _T(""), edit_flags, XSize(280, DEF_GUI_ROW_HEIGHT));
		
		sizer.addWidget(new XLabel(), _T("Інформатор повн: "), FL_WINDOW_VISIBLE, \
						XSize(125, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *informer_full = new XTabStopEdit(this);
        inserter.addFieldValueHolder("informer_full_name", informer_full);
		sizer.addWidget(informer_full, _T(""), edit_flags, XSize(380, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Місцезнаходження:"), FL_WINDOW_VISIBLE, \
						XSize(130, DEF_GUI_ROW_HEIGHT + 10));
		CDbComboBox* distr = new CDbComboBox(districts_list->getMasterResultSet(), 2, 0);
        inserter.setLocationsWidget(distr);
        sizer.addResizeableWidget(distr, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(190, DEF_GUI_ROW_HEIGHT), 150);

		sizer.addWidget(new XLabel(), _T("Тип:"), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT + 10));
		XTabStopComboBox* org_type = new XTabStopComboBox(this);
        inserter.addFieldValueHolder("inf_class", org_type);
		sizer.addResizeableWidget(org_type, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
						FL_COMBOBOX_DROPDOWN, XSize(170, DEF_GUI_ROW_HEIGHT), 150);

		org_type->AddItem(_T("Правоохоронні органи"));
		org_type->AddItem(_T("Пенітенціарні установи"));
		org_type->AddItem(_T("Суди"));
		org_type->AddItem(_T("Інші установи"));
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

	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_location"));
	grid->SetFocus();

	grid_x = grid_coords.left;
	grid_y = grid_coords.top;

	XRect margins = main_sizer.getMargins();
	grid_margin_x = margins.left;
	grid_margin_y = margins.top;

    inserter.prepare(conn);
}

void InformersBook::initFilteringControls() {

	flt_id = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> id_binder = std::make_shared<CIntWidgetBinderControl>(flt_id);
	
	ImmutableString<char> expr("inf.id_inf = ?", sizeof("inf.id_inf = ?") - 1);
	int id_expr = filtering_manager.addExpr(expr, id_binder);
	flt_id->setExprId(id_expr);

	flt_informer = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> inf_binder = std::make_shared<CStringWidgetBinderControl>(flt_informer);

	ImmutableString<char> expr2("inf.informer_name LIKE ?", sizeof("inf.informer_name LIKE ?") - 1);
	int inf_expr = filtering_manager.addExpr(expr2, inf_binder);
	flt_informer->setExprId(inf_expr);
}

void InformersBook::OnFilterButtonClick(XCommandEvent *eve) {

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

void InformersBook::OnAddRecordButtonClick(XCommandEvent *eve) {

    if (inserter.insert())
        db_table->reload();
}

void InformersBook::OnRemoveButtonClick(XCommandEvent *eve) {

	int option = _plMessageBoxYesNo(_T("Видалити поточний запис?"));
	if (option == IDYES) {
		try {
			db_table->removeCurrentRecord();
		}
		catch (CDbException& e) {
			if (e.E_DB_PARENT_NO_ROW_UPD_DEL)
				ErrorBox(_T("до цього інформатора прив'язані доручення, тому видалення неможливе"));
			else
				ErrorBox(e.what());
		}
	}
}

void InformersBook::OnSize(XSizeEvent *eve) {

	int width = eve->GetWidth();
	int height = eve->GetHeight();

	if (!(grid && width && height)) return;
		
	grid->MoveWindow(grid_x, grid_y, \
					width - 2 * grid_margin_x, \
					height - grid_y - grid_margin_y);
}

InformersBook::~InformersBook() {

	if (grid && !grid->IsCreated()) delete grid;
	if (flt_id && !flt_id->IsCreated()) delete flt_id;
	if (flt_informer && !flt_informer->IsCreated()) delete flt_informer;
}
