#pragma once
#include "EditableGrid.h"
#include <table/StringTable.h>

class CStringGrid :	public CEditableGrid{
	std::shared_ptr<CStringTable> table;
public:
	CStringGrid();
	void SetCell(const size_t field, const size_t record, \
					const Tchar *value);

	void AddField(const Tchar *name, const int max_width_in_chars);
	void AddRecord();

	virtual ~CStringGrid();
};

