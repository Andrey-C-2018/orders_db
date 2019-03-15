#include <db_controls/DbGrid.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <properties/Properties.h>
#include "AdvocatsBook.h"

CAdvocatsBook::CAdvocatsBook() {

	CProperties props("config.ini");

	auto conn = createConnection(props);
	auto db_table = createDbTable(conn);
	
	grid = new CDbGrid(db_table);
	createCellWidgetsAndAttachToGrid(grid);
}
	
std::shared_ptr<IDbConnection> CAdvocatsBook::createConnection(CProperties &props) {

	std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();

	conn->Connect(props.getStringProperty(_T("server_address")), \
			props.getIntProperty(_T("port")), \
			props.getStringProperty(_T("user")), \
			props.getStringProperty(_T("password")), \
			props.getStringProperty(_T("database")));
	return conn;
}

std::shared_ptr<CDbTable> CAdvocatsBook::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_advocat, b.adv_name, b.license_no, b.license_date,";
	query += " e.examiner_name, post_index, address, adv_bdate,";
	query += " d.distr_center, b.org_name, b.org_type "
	query += "FROM advocats b INNER JOIN examiners e ON b.id_exm = a.id_examiner ";
	query += "ORDER BY adv_name";

	filter.setConnection(conn);
	filter.setFilteredQuery(query);

	auto stmt = filter.applyFilter();

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("advocats");
	db_table->markFieldAsPrimaryKey("id_advocat");

	return db_table;
}

void CAdvocatsBook::createCellWidgetsAndAttachToGrid(CEditableGrid *grid) {

	assert(grid);
	CDbComboBoxCellWidget *examiners_list = new CDbComboBoxCellWidget(conn, "exm_name", \
									"examiners", "advocats", db_table);
	examiners_list->AddRelation("id_examiner", "id_exm");
	grid->SetWidgetForField("examiner_name", examiners_list);

	CComboBoxCellWidget *adv_org_types_list = new CComboBoxCellWidget();
	adv_org_types_list->AddItem(_T("²ÍÄ"));
	adv_org_types_list->AddItem(_T("ÀÎ"));
	adv_org_types_list->AddItem(_T("ÀÁ"));
	grid->SetWidgetForField("org_type", adv_org_types_list);
}

void CAdvocatsBook::Create(XWindow *parent, const int flags,
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) {

	XRect rc;

	GetClientRect(rc);

	auto flt_widgets = createFilterWidgetsAndPackToSizer();
	flt_widgets->add(grid);

	flt_widgets.Create(this, 10, 10, rc.right - 20, rc.bottom - 20);
	grid->SetFocus();
}

CVerticalSizer CAdvocatsBook::createFilterWidgetsAndPackToSizer() {

	CVerticalSizer main_sizer;
	main_sizer.add(CHorizontalSizer(), 5);
	
	auto stmt = conn.PrepareQuery("SELECT adv_name FROM advocats ORDER BY 1");
	size_t i = 0;
	{
		flt_advocats = new CDbComboBox(stmt);
		matchWidgetToFilter(flt_advocats, "b.id_advocat = ");
		flt_advocats->ConnectOnChangeHandler(this, &CAdvocatsBook::OnFilteringWidgetChanged);
		main_sizer[i].add(flt_advocats);

		flt_bdate = new CValidatedEdit(VALIDATE_ON_LOOSE_FOCUS);
		matchWidgetToFilter(flt_bdate, "b.bdate = ");
		flt_bdate->setValidator(CDateValidator());
		main_sizer[i].add(flt_bdate);

		stmt = conn.PrepareQuery("SELECT district_center FROM districts ORDER BY 1");
		flt_district = new CDbComboBox(stmt);
		matchWidgetToFilter(flt_district, "b.id_main_district = ");
		main_sizer[i].add(flt_district);
	}
	++i;
	{
		flt_license_no = new CValidatedEdit(VALIDATE_ON_LOOSE_FOCUS);
		matchWidgetToFilter(flt_license_no, "b.license_no = ");
		flt_license_no->setValidator(CAdvLicenseNoValidator());
		main_sizer[i].add(flt_license_no);

		flt_license_date = new CValidatedEdit(VALIDATE_ON_LOOSE_FOCUS);
		matchWidgetToFilter(flt_license_date, "b.license_date = ");
		flt_license_date->setValidator(CDateValidator());
		main_sizer[i].add(flt_license_date);

		stmt = conn.PrepareQuery("SELECT exm_name FROM examiners ORDER BY 1");
		flt_examiner = new CDbComboBox(stmt);
		matchWidgetToFilter(flt_examiner, "b.id_exm = ");
		main_sizer[i].add(flt_advocats);
	}
	++i;
	{
		flt_post_index = new CValidatedEdit(VALIDATE_ON_LOOSE_FOCUS);
		flt_post_index->setValidator(CPostIndexValidator());
		main_sizer[i].add(flt_post_index);

		flt_address = new XEdit();
		main_sizer[i].add(flt_address);

		flt_tel = new CValidatedEdit(VALIDATE_ON_LOOSE_FOCUS);
		flt_tel->setValidator(CTelephoneValidator());
		main_sizer[i].add(flt_tel);

		flt_email = new CValidatedEdit(VALIDATE_ON_LOOSE_FOCUS);
		flt_email->setValidator(CEmailValidator());
		main_sizer[i].add(flt_email);
	}
	++i;
	{
		flt_entr_type = new XComboBox();
		flt_entr_type->addItem(_T("ÀÎ"));
		flt_entr_type->addItem(_T("ÀÁ"));
		flt_entr_type->addItem(_T("²ÍÄ"));
		main_sizer[i].add(flt_entr_type);

		flt_entr_name = new XEdit();
		main_sizer[i].add(flt_entr_name);
	}

	return std::move(main_sizer);
}

void CAdvocatsBook::OnFilteringWidgetChanged(XCommandEvent *eve) {

	size_t index = eve->GetTag();
	assert(index < filtering_items.size());

	size_t param_no = filter.setFiltering(filtering_items[index].expression);
	auto stmt = filter.getStatement();

	ImmutableString<Tchar> value = filtering_items[index].value_holder->getValue();
	filtering_items[index].value_binder->bindValueAsWString(stmt, param_no, value.str);
}

void CAdvocatsBook::OnFilterButtonClick(XCommandEvent *eve) {

	db_table.reload(filter.applyFilter());
}

void CAdvocatsBook::OnOrderingButtonClick(XCommandEvent *eve) {

	db_table.reload();
}

void CAdvocatsBook::OnRemoveButtonClick(XCommandEvent *eve) {

}

void CAdvocatsBook::OnUploadButtonClick(XCommandEvent *eve) {

}

CAdvocatsBook::~CAdvocatsBook() { }
