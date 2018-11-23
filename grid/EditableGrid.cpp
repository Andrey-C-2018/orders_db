#include "EditableGrid.h"
#include "EditableTextCell.h"

class CEConfigurator : public IConfigurator {
	CEditableTextCell *editable_cell;
public:
	CEConfigurator() : editable_cell(nullptr) { }

	void Configure(CGridLine &line) override { }

	void Configure(CTextCell &text_cell) override {	}

	void Configure(CEditableTextCell &editable_cell_) override {

		this->editable_cell = &editable_cell_;
	}

	inline CEditableTextCell *GetEditableTextCell() const {

		return editable_cell;
	}

	virtual ~CEConfigurator() { }
};

//**************************************************************

CEditableGrid::CEditableGrid() : CGrid(), editable_cell(nullptr){ }

LayoutObjects CEditableGrid::CreateLayoutObjects(const int kind_of_layout) {
	LayoutObjects obj;
	return CreateLayoutObjectsHelper<CEditableTextCell>(kind_of_layout);
}

void CEditableGrid::OnWindowCreate() {
	CEConfigurator configurator;

	AcceptConfiguratorOnCells(&configurator);
	editable_cell = configurator.GetEditableTextCell();

	Connect(EVT_KEYDOWN, this, &CEditableGrid::OnKeyPress);
}

void CEditableGrid::OnKeyPress(XKeyboardEvent *eve) {
	assert(editable_cell);
	size_t visible_records_count = GetVisibleRecordsCount();
	size_t active_record = editable_cell->GetActiveRecord();
	size_t records_count = GetRecordsCount();

	if (records_count == 0) return;

	size_t new_record = 0;
	bool Key = false;

	switch (eve->GetKey()) {
		case X_VKEY_UP:
			new_record = active_record ? active_record - 1 : 0;
			Key = true;
		break;

		case X_VKEY_DOWN:
			new_record = active_record + 1 >= records_count ? \
							records_count - 1 : active_record + 1;
			Key = true;
		break;

		case X_VKEY_PGUP:
			new_record = (active_record >= visible_records_count) ? \
							active_record - visible_records_count : 0;
			Key = true;
		break;

		/*case X_VKEY_RIGHT:
			Key = GotoNextCol();
		break;

		case X_VKEY_LEFT:
			Key = GotoPrevCol();
		break;*/

		case X_VKEY_PGDOWN:
			new_record = (active_record + visible_records_count) >= records_count ? \
							records_count - 1 : active_record + visible_records_count;
			Key = true;
		break;

		case X_VKEY_ENTER:
			editable_cell->SetFocus();
	}

	if (Key && new_record != active_record) {
		size_t active_field = editable_cell->GetActiveField();

		editable_cell->OnClick(active_field, new_record);
		FocusOnRecord(new_record);
	}
}

CEditableGrid::~CEditableGrid(){ }
