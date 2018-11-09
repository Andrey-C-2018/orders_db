#include "StringGrid.h"

CStringGridException::CStringGridException(const int err_code, const Tchar *err_descr) : \
											XException(err_code, err_descr) { }

CStringGridException::CStringGridException(const CStringGridException &obj) : XException(obj) { }

CStringGridException::~CStringGridException() { }

//****************************************************************************

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

	if (table->empty())
		throw CStringGridException(CStringGridException::E_EMPTY_GRID, \
									_T("The grid is empty"));

	table->AddRecord();
}

CStringGrid::~CStringGrid(){ }
