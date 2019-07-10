#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_controls/DbGrid.h>
#include "AdvocatsListManager.h"
#include "AdvocatsListEvtHandler.h"

CAdvocatsListManager::CAdvocatsListManager() : grid(nullptr) { }

void CAdvocatsListManager::Init(std::shared_ptr<IDbConnection> conn_, XWindow *parent) {

	assert(!db_table);
	assert(!grid);

	db_table = createDbTable(conn_);
	grid = new CDbGrid(db_table, std::make_shared<CAdvocatsListEvtHandler>(db_table));
}

std::shared_ptr<CDbTable> CAdvocatsListManager::createDbTable(std::shared_ptr<IDbConnection> conn) {

	std::string query = "SELECT b.id_advocat, b.adv_name_short ";
	query += "FROM advocats b ";
	query += "ORDER BY adv_name_short";

	auto stmt = conn->PrepareQuery(query.c_str());

	auto db_table = std::make_shared<CDbTable>(conn, CQuery(conn, stmt));
	db_table->setPrimaryTableForQuery("advocats");
	db_table->markFieldAsPrimaryKey("id_advocat");

	return db_table;
}

void CAdvocatsListManager::CreateWidgets(XWindow *parent, const int flags, \
										const Tchar *label, \
										const int x, const int y, \
										const int width, const int height) {

	grid->Create(parent, flags, _T(""), x, y, width, height);
}

CAdvocatsListManager::~CAdvocatsListManager() {

	if (grid && !grid->IsCreated()) delete grid;
}
