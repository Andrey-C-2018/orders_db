#include "StringGrid.h"
#include "IGridEventsHandler.h"

CStringGridException::CStringGridException(const int err_code, const Tchar *err_descr) : \
											XException(err_code, err_descr) { }

CStringGridException::CStringGridException(const CStringGridException &obj) : XException(obj) { }

CStringGridException::~CStringGridException() { }

//****************************************************************************

class CStringGridEventsHandler final : public IGridEventsHandler {

	size_t active_field, active_record;
public:

	CStringGridEventsHandler() : active_field(0), active_record(0) { }

	bool OnCellChanged(IGridCellWidget *cell_widget) override {

		return true;
	}

	bool OnCellChangedIndirectly(IGridCellWidget *cell_widget) override {

		return true;
	}

	void OnActiveCellLocationChanged(const size_t new_field, \
									const size_t new_record) override {

		active_field = new_field;
		active_record = new_record;
	}

	inline size_t getActiveField() const { return active_field; }
	inline size_t getActiveRecord() const { return active_record; }
};

//****************************************************************************

CStringGrid::CStringGrid() : \
			CEditableGrid(std::unique_ptr<IGridEventsHandler>(new CStringGridEventsHandler())), \
			table(std::make_shared<CStringTable>()){

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
