#include "StringGrid.h"

CStringGrid::CStringGrid() : table(std::make_shared<CStringTable>()){

	Init(table, LAYOUT_RECORD);
}

void CStringGrid::SetCell(const size_t field, const size_t record, \
							const Tchar *value) {

	table->SetCell(field, record, value);

	if (IsCreated()) 
		Invalidate(nullptr, false);
}

void CStringGrid::AddField(const Tchar *name, const int max_width_in_chars) {

	assert(max_width_in_chars != 0);
	assert(name);

	table->AddField(max_width_in_chars, name);
}

void CStringGrid::AddRecord() {

	table->AddRecord();
}

CStringGrid::~CStringGrid(){ }
