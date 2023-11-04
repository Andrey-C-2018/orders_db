#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <editable_grid/DateCellWidget.h>
#include <db_controls/DbGrid.h>
#include <db_controls/DbComboBox.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <db_controls/BinderControls.h>
#include <db_controls/FilteringEdit.h>
#include <db_controls/DbNavigator.h>
#include <xwindows/XLabel.h>
#include <xwindows_ex/HorizontalSizer.h>
#include <xwindows_ex/VerticalSizer.h>
#include <xwindows_ex/XTabStopWidget.h>
#include <xwindows_ex/XTabStopEdit.h>
#include <xwindows_ex/XDateField.h>
#include <forms_common/VernamOneTimePad.h>
#include "AdvocatsBook.h"
#include "AdvocatsGridEvtHandler.h"
#include "PostIndexCellWidget.h"

typedef XTabStopWidget<XComboBox> XTabStopComboBox;

CAdvocatsBook::CAdvocatsBook(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int X, const int Y, \
							const int width, const int height) : \
	flt_id(nullptr), flt_name(nullptr), \
	btn_apply_filter(nullptr), btn_add(nullptr), btn_remove(nullptr), \
	grid(nullptr), examiners_list(nullptr), adv_org_types_list(nullptr), districts_list(nullptr), \
	grid_x(0), grid_y(0), grid_margin_x(0), grid_margin_y(0) {

	props.open();

	initFilteringControls();

	conn = CMySQLConnectionFactory::createConnection(props, vernam_one_time_pad);
	db_table = createDbTable(conn);
	
	grid = new CDbGrid(false, db_table, std::make_shared<CAdvocatsGridEvtHandler>(db_table));
	setFieldsSizes();
	createCellWidgetsAndAttachToGrid(grid);

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);
	DisplayWidgets();
	adjustUIDependentCellWidgets(grid);

	Connect(EVT_COMMAND, btn_apply_filter->GetId(), this, &CAdvocatsBook::OnFilterButtonClick);
	Connect(EVT_COMMAND, btn_add->GetId(), this, &CAdvocatsBook::OnAddRecordButtonClick);
	Connect(EVT_COMMAND, btn_remove->GetId(), this, &CAdvocatsBook::OnRemoveButtonClick);
}
	
std::shared_ptr<CDbTable> CAdvocatsBook::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_advocat, p.name, p.name_short, b.license_no,";
	query += " b.license_date, e.exm_name, b.post_index, b.address, b.tel, b.email, p.bdate,";
	query += " d.distr_center, b.org_name, b.org_type, b.id_exm, b.id_main_district ";
	query += "FROM advocats b INNER JOIN people p ON b.id_advocat = p.id_person";
	query += " INNER JOIN examiners e ON b.id_exm = e.id_examiner";
	query += " INNER JOIN districts d ON b.id_main_district = d.id_distr";
	query += " #### ";
	query += "ORDER BY name";
	query_modifier.Init(query);

	auto stmt = conn->PrepareQuery(query_modifier.getQuery().c_str());

	auto db_table = std::make_shared<CDbTable>(conn, stmt);
	db_table->setPrimaryTableForQuery("people");
	db_table->markFieldAsPrimaryKey("id_advocat", "advocats");

	db_table->addPrimaryKeyAsRef("id_person", "people", "id_advocat", "advocats");
	return db_table;
}

void CAdvocatsBook::setFieldsSizes() {

	const auto& meta_info = db_table->getMetaInfo();
	size_t field_index = meta_info.getFieldIndexByName("id_advocat");

	grid->SetFieldWidth(field_index, 4);
	grid->SetFieldLabel(field_index, _T("ID"));

	field_index = meta_info.getFieldIndexByName("name");
	grid->SetFieldWidth(field_index, 37);
	grid->SetFieldLabel(field_index, _T("ПІБ адвоката"));

	field_index = meta_info.getFieldIndexByName("name_short");
	grid->SetFieldWidth(field_index, 25);
	grid->SetFieldLabel(field_index, _T("ПІБ коротке"));

	field_index = meta_info.getFieldIndexByName("license_no");
	grid->SetFieldWidth(field_index, 10);
	grid->SetFieldLabel(field_index, _T("№ св."));

	field_index = meta_info.getFieldIndexByName("license_date");
	grid->SetFieldWidth(field_index, 10);
	grid->SetFieldLabel(field_index, _T("Дата св."));

	field_index = meta_info.getFieldIndexByName("exm_name");
	grid->SetFieldWidth(5, 70);
	grid->SetFieldLabel(5, _T("Орган, який видав свідоцтво"));

	field_index = meta_info.getFieldIndexByName("post_index");
	grid->SetFieldWidth(field_index, 11);
	grid->SetFieldLabel(field_index, _T("Пошт індекс"));

	field_index = meta_info.getFieldIndexByName("address");
	grid->SetFieldWidth(meta_info.getFieldIndexByName("address"), 75);
	grid->SetFieldLabel(field_index, _T("Адреса"));

	grid->SetFieldWidth(meta_info.getFieldIndexByName("tel"), 25);
	grid->SetFieldWidth(meta_info.getFieldIndexByName("email"), 20);

	field_index = meta_info.getFieldIndexByName("bdate");
	grid->SetFieldWidth(field_index, 12);
	grid->SetFieldLabel(field_index, _T("Дата народж"));

	field_index = meta_info.getFieldIndexByName("distr_center");
	grid->SetFieldWidth(field_index, 25);
	grid->SetFieldLabel(field_index, _T("Основний район роботи"));

	field_index = meta_info.getFieldIndexByName("org_name");
	grid->SetFieldWidth(field_index, 55);
	grid->SetFieldLabel(field_index, _T("Назва організації"));

	field_index = meta_info.getFieldIndexByName("org_type");
	grid->SetFieldWidth(field_index, 12);
	grid->SetFieldLabel(field_index, _T("Тип орг."));
}

void CAdvocatsBook::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {

	assert(grid);
	assert(!examiners_list);
	assert(!districts_list);
	assert(!adv_org_types_list);
	CEditableCellWidget* readonly_widget = nullptr;
	CPostIndexCellWidget *post_index_widget = nullptr;
	CDateCellWidget *date_widget = nullptr;

	bool exm = false, post = false, distr = false;
	bool adv = false, date = false, readonly = false;
	try {
		readonly_widget = new CEditableCellWidget(true);
		grid->SetWidgetForFieldByName("id_advocat", readonly_widget);
		readonly = true;

		examiners_list = new CDbComboBoxCellWidget(conn, "exm_name", \
										"examiners", "advocats", db_table);
		examiners_list->AddRelation("id_examiner", "id_exm");
		grid->SetWidgetForFieldByName("exm_name", examiners_list);
		exm = true;

		post_index_widget = new CPostIndexCellWidget();
		grid->SetWidgetForFieldByName("post_index", post_index_widget);
		post = true;

		districts_list = new CDbComboBoxCellWidget(conn, "distr_center", \
										"districts", "advocats", db_table);
		districts_list->AddRelation("id_distr", "id_main_district");
		grid->SetWidgetForFieldByName("distr_center", districts_list);
		distr = true;

		adv_org_types_list = new CComboBoxCellWidget();
		grid->SetWidgetForFieldByName("org_type", adv_org_types_list);
		adv = true;

		date_widget = new CDateCellWidget();
		grid->SetWidgetForFieldByName("license_date", date_widget);
		date = true;

		grid->SetWidgetForFieldByName("bdate", date_widget);
	}
	catch (...) {
		if (!exm && examiners_list && !examiners_list->IsCreated()) 
			delete examiners_list;
		if (!post && post_index_widget && !post_index_widget->IsCreated())
			delete post_index_widget;
		if (!distr && districts_list && !districts_list->IsCreated()) 
			delete districts_list;
		if (!adv && adv_org_types_list && !adv_org_types_list->IsCreated())
			delete adv_org_types_list;
		if (!date && date_widget && !date_widget->IsCreated())
			delete date_widget;
		if (!readonly && readonly_widget && !readonly_widget->IsCreated())
			delete readonly_widget;

		throw;
	}
}

void CAdvocatsBook::adjustUIDependentCellWidgets(CDbGrid *grid) {

	assert(adv_org_types_list);
	adv_org_types_list->AddItem(_T("ІНД"));
	adv_org_types_list->AddItem(_T("АО"));
	adv_org_types_list->AddItem(_T("АБ"));
}

void CAdvocatsBook::DisplayWidgets() {

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
		adv_inserter.setAdvNameWidget(flt_name);
		sizer.addWidget(flt_name, _T(""), edit_flags, XSize(280, DEF_GUI_ROW_HEIGHT));
		
		sizer.addWidget(new XLabel(), _T("№ свід.: "), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT + 10));
		XTabStopEdit *license_no = new XTabStopEdit(this);
		adv_inserter.setLicenseNoWidget(license_no);
		sizer.addWidget(license_no, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Дата свід.: "), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT + 10));
		XDateField *license_date = new XDateField(this);
		adv_inserter.setLicenseDateWidget(license_date);
		sizer.addWidget(license_date, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(80, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Хто видав:"), FL_WINDOW_VISIBLE, \
						XSize(45, DEF_GUI_ROW_HEIGHT + 10));
		CDbComboBox *examiner = new CDbComboBox(examiners_list->getMasterResultSet(), 1, 0);
		adv_inserter.setExaminerWidget(examiner);
		sizer.addResizeableWidget(examiner, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(250, DEF_GUI_ROW_HEIGHT), 150);
		examiner->setTabStopManager(this);
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("Пошт. індекс:"), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT + 10));
		XTabStopEdit *post_index = new XTabStopEdit(this);
		adv_inserter.setPostIndexWidget(post_index);
		sizer.addWidget(post_index, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
						XSize(70, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Адреса:"), FL_WINDOW_VISIBLE, \
						XSize(53, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *address = new XTabStopEdit(this);
		adv_inserter.setAddressWidget(address);
		sizer.addWidget(address, _T(""), edit_flags, XSize(380, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Дата народж:"), FL_WINDOW_VISIBLE, \
						XSize(55, DEF_GUI_ROW_HEIGHT + 10));
		XDateField *bdate = new XDateField(this);
		adv_inserter.setBDateWidget(bdate);
		sizer.addWidget(bdate, _T(""), edit_flags, XSize(80, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Тел.:"), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *tel = new XTabStopEdit(this);
		adv_inserter.setTelWidget(tel);
		sizer.addWidget(tel, _T(""), edit_flags, XSize(200, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(new XLabel(), _T("E-mail:"), FL_WINDOW_VISIBLE, \
						XSize(50, DEF_GUI_ROW_HEIGHT));
		XTabStopEdit *email = new XTabStopEdit(this);
		adv_inserter.setEmailWidget(email);
		sizer.addWidget(email, _T(""), edit_flags, XSize(200, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Район роботи:"), FL_WINDOW_VISIBLE, \
						XSize(55, DEF_GUI_ROW_HEIGHT + 10));
		CDbComboBox *distr = new CDbComboBox(districts_list->getMasterResultSet(), 2, 0);
		adv_inserter.setDistrictWidget(distr);
		sizer.addResizeableWidget(distr, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
									XSize(190, DEF_GUI_ROW_HEIGHT), 150);

		sizer.addWidget(new XLabel(), _T("Назва орг.:"), FL_WINDOW_VISIBLE, \
						XSize(45, DEF_GUI_ROW_HEIGHT + 10));
		XTabStopEdit *org = new XTabStopEdit(this);
		adv_inserter.setOrgNameWidget(org);
		sizer.addWidget(org, _T(""), edit_flags, XSize(200, DEF_GUI_ROW_HEIGHT));

		sizer.addWidget(new XLabel(), _T("Тип орг.:"), FL_WINDOW_VISIBLE, \
						XSize(40, DEF_GUI_ROW_HEIGHT + 10));
		XTabStopComboBox *org_type = new XTabStopComboBox(this);
		adv_inserter.setOrgTypeWidget(org_type);
		sizer.addResizeableWidget(org_type, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
									FL_COMBOBOX_DROPDOWN, XSize(50, DEF_GUI_ROW_HEIGHT), 150);
		org_type->AddItem(_T("ІНД"));
		org_type->AddItem(_T("АО"));
		org_type->AddItem(_T("АБ"));
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

	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_exm"));
	grid->HideField(db_table->getMetaInfo().getFieldIndexByName("id_main_district"));
	grid->SetFocus();

	grid_x = grid_coords.left;
	grid_y = grid_coords.top;

	XRect margins = main_sizer.getMargins();
	grid_margin_x = margins.left;
	grid_margin_y = margins.top;

	adv_inserter.prepare(conn);
}

void CAdvocatsBook::initFilteringControls() {

	flt_id = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> id_binder = std::make_shared<CIntWidgetBinderControl>(flt_id);
	
	ImmutableString<char> expr("b.id_advocat = ?", sizeof("b.id_advocat = ?") - 1);
	int id_expr = filtering_manager.addExpr(expr, id_binder);
	flt_id->setExprId(id_expr);

	flt_name = new CFilteringEdit(filtering_manager, this);
	std::shared_ptr<IBinder> name_binder = std::make_shared<CStringWidgetBinderControl>(flt_name);

	ImmutableString<char> expr2("p.name LIKE ?", sizeof("p.name LIKE ?") - 1);
	int name_expr = filtering_manager.addExpr(expr2, name_binder);
	flt_name->setExprId(name_expr);
}

void CAdvocatsBook::OnFilterButtonClick(XCommandEvent *eve) {

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

void CAdvocatsBook::OnAddRecordButtonClick(XCommandEvent *eve) {

	if (adv_inserter.insert())
		db_table->reload();
}

void CAdvocatsBook::OnRemoveButtonClick(XCommandEvent *eve) {

	int option = _plMessageBoxYesNo(_T("Видалити поточний запис?"));
	if (option == IDYES) {
		try {
			db_table->removeCurrentRecord();
		}
		catch (CDbException& e) {
			if (e.E_DB_PARENT_NO_ROW_UPD_DEL)
				ErrorBox(_T("до цього адвоката прив'язані доручення, тому видалення неможливе"));
			else
				ErrorBox(e.what());
		}
	}
}

void CAdvocatsBook::OnSize(XSizeEvent *eve) {

	int width = eve->GetWidth();
	int height = eve->GetHeight();

	if (!(grid && width && height)) return;
		
	grid->MoveWindow(grid_x, grid_y, \
					width - 2 * grid_margin_x, \
					height - grid_y - grid_margin_y);
}

CAdvocatsBook::~CAdvocatsBook() {

	if (grid && !grid->IsCreated()) delete grid;
	if (flt_id && !flt_id->IsCreated()) delete flt_id;
	if (flt_name && !flt_name->IsCreated()) delete flt_name;
}
