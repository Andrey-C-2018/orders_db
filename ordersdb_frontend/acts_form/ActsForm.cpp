#include <basic/XConv.h>
#include <db/MySQL/MySQLConnectionFactory.h>
#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_ext/DependentTableUpdater.h>
#include <xwindows/XButton.h>
#include <forms_common/PaymentsDbTableEvtHandler.h>
#include <forms_common/ParametersManager.h>
#include <forms_common/CommonRoutines.h>
#include <forms_common/VernamOneTimePad.h>
#include "ActsForm.h"
#include "DefenderDbTableEvtHandler.h"
#include "OrdersDbTableEventsHandler.h"

constexpr char acts_form_version[] = "1.1.0";

CActsForm::CActsForm(XWindow *parent, const int flags, \
					const Tchar *label, \
					const int X, const int Y, \
					const int width, const int height) : \
				defenders_list(DEF_GUI_MARGIN, DEFENDERS_LIST_WIDTH, DEF_DBNAVIGATOR_HEIGHT), \
				orders_list(DEF_GUI_MARGIN, 0.5F, DEF_DBNAVIGATOR_HEIGHT), \
				payments_list(DEF_GUI_MARGIN, DEF_DBNAVIGATOR_HEIGHT) {

	props.open();
	conn = CMySQLConnectionFactory::createConnection(props, vernam_one_time_pad);
	auto version = getFormVersion(conn, "acts_form_version");
	if (version != acts_form_version)
		throw XException(0, _T("Версія програми не співпадає з версією БД. Використовуйте актуальну версію"));

	CParametersManager::init(&props, conn);

	setLastChangedUser();

	defenders_list.initDbTable(conn);
	auto defenders_db_table = defenders_list.getDbTable();
	size_t id_person_index = defenders_db_table->getMetaInfo().getFieldIndexByName("id_person");

	constraints = std::make_shared<CPaymentsConstraints>();
	constraints->old_stage_locked = true;
	constraints->wrong_zone = true;
	constraints->old_order_locked = true;

	bool is_null;
	const int defender_id = defenders_db_table->getResultSet()->getInt(id_person_index, is_null);
	assert(!is_null);
	orders_list.initDbTable(conn, defender_id, constraints);
		
	auto orders_db_table = orders_list.getDbTable();
	auto defender_evt_handler = std::make_shared<DefenderDbTableEvtHandler>(defenders_db_table, \
																orders_db_table, id_person_index, 0);
	defenders_list.initDbTableEvtHandler(defender_evt_handler);

	size_t orders_prim_key[3];
	orders_prim_key[0] = orders_db_table->getMetaInfo().getFieldIndexByName("id_center_legalaid");
	orders_prim_key[1] = orders_db_table->getMetaInfo().getFieldIndexByName("id");
	orders_prim_key[2] = orders_db_table->getMetaInfo().getFieldIndexByName("order_date");
	
	int def_center = 0, def_order_no = 0;
	CDate def_order_date;
	if (!orders_db_table->empty()) {
		auto orders_rs = orders_db_table->getResultSet();

		def_center = orders_rs->getInt(orders_prim_key[0], is_null);
		assert(!is_null);
		def_order_no = orders_rs->getInt(orders_prim_key[1], is_null);
		assert(!is_null);
		def_order_date = orders_rs->getDate(orders_prim_key[2], is_null);
		assert(!is_null);
	}
	payments_list.initDbTable(conn, def_center, def_order_no, \
									def_order_date, constraints);

	size_t params[3] = {0, 1, 2};
	auto payments_db_table = payments_list.getDbTable();
	auto orders_evt_handler = std::make_shared<COrdersDbTableEventsHandler>(orders_db_table, \
													payments_db_table, orders_prim_key, params);
	orders_list.initDbTableEvtHandler(orders_evt_handler);

	def_center = CParametersManager::getInstance().getDefaultCenter();
	auto payments_evt_handler = std::make_shared<CPaymentsDbTableEvtHandler>(payments_db_table, \
										def_center, "id_center", \
										true, true, true, constraints);
	payments_list.initDbTableEvtHandler(payments_evt_handler);

	Create(parent, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	XRect rc;
	GetClientRect(rc);

	defenders_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom));
	orders_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom), &defenders_list.getFirstSizer());
	payments_list.initSizers(XPoint(DEF_GUI_MARGIN, DEF_GUI_MARGIN), \
						XSize(rc.right, rc.bottom), &orders_list.getLastSizer());

	defenders_list.displayWidgets(this);
	orders_list.displayWidgets(this);
	payments_list.displayWidgets(this);
}

void CActsForm::OnSize(XSizeEvent *eve) {

	if(!defenders_list.updateSizers(XSize(eve->GetWidth(), eve->GetHeight()))) return;
	orders_list.updateSizers();
	payments_list.updateSizers();

	defenders_list.resize();
	orders_list.resize();
	payments_list.resize();
}

CActsForm::~CActsForm() { }
