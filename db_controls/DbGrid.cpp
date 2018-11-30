#include "DbGrid.h"

CDbGrid::CDbGrid(std::shared_ptr<CDbTable> db_table_) : db_table(db_table_) {

	Init(db_table, LAYOUT_FIELD);
}

CDbGrid::~CDbGrid(){ }
