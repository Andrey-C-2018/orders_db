#pragma once
#include "EditableGrid.h"
#include <table/StringTable.h>

class CStringGridException : public XException {
public:
	enum Errors {
		E_EMPTY_GRID = 1
	};

	CStringGridException(const int err_code, const Tchar *err_descr);
	CStringGridException(const CStringGridException &obj);
	CStringGridException(CStringGridException &&obj) = default;
	~CStringGridException();
};

class CStringGrid :	public CEditableGrid {
	std::shared_ptr<CStringTable> table;

	CStringGrid(const CStringGrid &obj) = delete;
	CStringGrid &operator=(const CStringGrid &obj) = delete;
public:
	CStringGrid();

	CStringGrid(CStringGrid &&obj) = default;
	CStringGrid &operator=(CStringGrid &&obj) = default;

	void SetCell(const size_t field, const size_t record, \
					const Tchar *value);

	void AddField(const Tchar *name, const int max_width_in_chars);
	void AddRecord();

	virtual ~CStringGrid();
};

