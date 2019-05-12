#include <db_controls/DbGrid.h>
#include <db_controls/DbComboBoxCellWidget.h>
#include <db/MySQL/MySQLConnection.h>
#include <xwindows/HorizontalSizer.h>
#include <xwindows/VerticalSizer.h>
#include "AdvocatsBook.h"

CAdvocatsBook::CAdvocatsBook() {

	props.open("config.ini");

	conn = createConnection(props);
	db_table = createDbTable(conn);
	
	grid = new CDbGrid(db_table);
	createCellWidgetsAndAttachToGrid(grid);
}
	
std::shared_ptr<IDbConnection> CAdvocatsBook::createConnection(const CPropertiesFile &props) {

	std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();

	Tstring buffer;
	const Tchar *curr_prop = nullptr;
	std::string server, user, pwd, database;
	
	curr_prop = props.getStringProperty(_T("server_address"), buffer);
	const char *p_server = UCS16_ToUTF8(curr_prop, -1, server);

	int port = props.getIntProperty(_T("port"), buffer);

	curr_prop = props.getStringProperty(_T("user"), buffer);
	const char *p_user = UCS16_ToUTF8(curr_prop, -1, user);

	curr_prop = props.getStringProperty(_T("password"), buffer);
	const char *p_pwd = UCS16_ToUTF8(curr_prop, -1, pwd);

	curr_prop = props.getStringProperty(_T("database"), buffer);
	const char *p_database = UCS16_ToUTF8(curr_prop, -1, database);

	conn->Connect(p_server, port, p_user, p_pwd, p_database);
	return std::move(conn);
}

std::shared_ptr<CDbTable> CAdvocatsBook::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_advocat, b.adv_name, b.license_no, b.license_date,";
	query += " e.examiner_name, post_index, address, adv_bdate,";
	query += " d.distr_center, b.org_name, b.org_type ";
	query += "FROM advocats b INNER JOIN examiners e ON b.id_exm = a.id_examiner ";
	query += "WHERE ";
	query += filtering_manager.getSQLWherePart().str;
	query += " ORDER BY adv_name";
	auto stmt = conn->PrepareQuery(query.c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("advocats");
	db_table->markFieldAsPrimaryKey("id_advocat");

	return std::move(db_table);
}

void CAdvocatsBook::createCellWidgetsAndAttachToGrid(CDbGrid *grid) {

	assert(grid);
	auto examiners_list = new CDbComboBoxCellWidget(conn, "exm_name", \
												"examiners", "advocats", db_table);
	examiners_list->AddRelation("id_examiner", "id_exm");
	grid->SetWidgetForField("examiner_name", examiners_list);

	auto adv_org_types_list = new CComboBoxCellWidget();
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
	CVerticalSizer main_sizer(this, 0, 0, rc.right, rc.bottom, \
								10, 10, 10, 10, \
								DEF_GUI_VERT_GAP, DEF_GUI_ROW_HEIGHT);

	CHorizontalSizer sizer(CSizerPreferences(0, 0, 0, 0, DEF_GUI_HORZ_GAP));
	main_sizer.pushNestedSizer(sizer);
	btn_apply_filter = new XButton();
	sizer.addWidget(btn_apply_filter, _T("Ô³ëüòð"), FL_WINDOW_VISIBLE, \
					XSize(100, DEF_GUI_ROW_HEIGHT));
	btn_ordering = new XButton();
	sizer.addWidget(btn_ordering, _T("Ñîðò."), FL_WINDOW_VISIBLE, \
					XSize(100, DEF_GUI_ROW_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.addWidget(grid);
	grid->SetFocus();
}

void CAdvocatsBook::OnFilteringWidgetChanged(XCommandEvent *eve) {

}

void CAdvocatsBook::OnFilterButtonClick(XCommandEvent *eve) {

}

void CAdvocatsBook::OnOrderingButtonClick(XCommandEvent *eve) {

}

void CAdvocatsBook::OnRemoveButtonClick(XCommandEvent *eve) {

}

void CAdvocatsBook::OnUploadButtonClick(XCommandEvent *eve) {

}

CAdvocatsBook::~CAdvocatsBook() { }
