#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <xwindows/XButton.h>
#include "ActsForm.h"
#include "AdvDbTableEventsHandler.h"
#include "OrdersDbTableEventsHandler.h"
#include "ParametersManager.h"

CActsForm::CActsForm(XWindow *parent, const int flags, \
					const Tchar *label, \
					const int X, const int Y, \
					const int width, const int height) : \
				adv_list(DEF_GUI_MARGIN, ADV_LIST_WIDTH, DEF_DBNAVIGATOR_HEIGHT), \
				orders_list(DEF_GUI_MARGIN, 0.5F, DEF_DBNAVIGATOR_HEIGHT), \
				payments_list(DEF_GUI_MARGIN, DEF_DBNAVIGATOR_HEIGHT) {

	props.open("config.ini");
	conn = CMySQLConnectionFactory::createConnection(props);
	CParametersManager::init(&props);

	adv_list.initDbTable(conn);
	auto adv_db_table = adv_list.getDbTable();
	size_t id_advocat = adv_db_table->getQuery().getMetaInfo().getFieldIndexByName("id_advocat");

	bool is_null;
	const int def_adv_id = adv_db_table->getResultSet()->getInt(id_advocat, is_null);
	assert(!is_null);
	orders_list.initDbTable(conn, def_adv_id);
		
	auto orders_db_table = orders_list.getDbTable();
	auto adv_evt_handler = std::make_shared<CAdvDbTableEventsHandler>(adv_db_table, \
																orders_db_table, id_advocat, 0);
	adv_list.initDbTableEvtHandler(adv_evt_handler);

	payments_list.initDbTable(conn, 1, 1, CDate(1, 1, 1901));
	size_t orders_prim_key[3];
	orders_prim_key[0] = orders_db_table->getQuery().getMetaInfo().getFieldIndexByName("id_center_legalaid");
	orders_prim_key[1] = orders_db_table->getQuery().getMetaInfo().getFieldIndexByName("id");
	orders_prim_key[2] = orders_db_table->getQuery().getMetaInfo().getFieldIndexByName("order_date");

	size_t params[3] = {0, 1, 2};
	auto payments_db_table = payments_list.getDbTable();
	auto orders_evt_handler = std::make_shared<COrdersDbTableEventsHandler>(orders_db_table, \
													payments_db_table, orders_prim_key, params);
	orders_list.initDbTableEvtHandler(orders_evt_handler);

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	XRect rc;
	GetClientRect(rc);

	adv_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom));
	orders_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom), &adv_list.getFirstSizer());
	payments_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom), &orders_list.getLastSizer());

	adv_list.displayWidgets(this);
	orders_list.displayWidgets(this);
	payments_list.displayWidgets(this);
}

void CActsForm::OnSize(XSizeEvent *eve) {

	if(!adv_list.updateSizers(XSize(eve->GetWidth(), eve->GetHeight()))) return;
	orders_list.updateSizers();
	payments_list.updateSizers();

	adv_list.resize();
	orders_list.resize();
	payments_list.resize();
}

CActsForm::~CActsForm() { }
