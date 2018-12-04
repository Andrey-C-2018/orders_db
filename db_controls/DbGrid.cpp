#include "DbGrid.h"
#include <grid/TextCell.h>

CDbGrid::CDbGrid(std::shared_ptr<CDbTable> db_table_) : db_table(db_table_) {

	Init(db_table, LAYOUT_FIELD);
}

//LayoutObjects CDbGrid::CreateLayoutObjects(const int kind_of_layout) {
//	LayoutObjects obj;
//	return CreateLayoutObjectsHelper<CTextCell>(kind_of_layout);
//}

CDbGrid::~CDbGrid(){ }
